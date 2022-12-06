#include "streaming_merger.h"

#include "util.h"

void Node::fetch_from_buffer(std::vector<int32_t>& output) {
    std::lock_guard<std::mutex> l(_m);
    CHECK(!_buffers.empty());
    const auto front = _buffers.front();
    output.insert(output.end(), front.begin(), front.end());
    _buffers.pop();
    if (_buffers.empty()) {
        _is_buffer_empty = true;
    }
    _full_cv.notify_one();
}

void Node::enqueue_buffer(std::unique_lock<std::mutex>& l, bool force) {
    if (!force && _buffer.size() < _chunk_size) {
        return;
    }
    if (!_buffer.empty()) {
        if (_buffers.size() >= _max_buffer_size) {
            _full_cv.wait(l);
        }
        CHECK(_buffers.size() < _max_buffer_size);
        _buffers.push(std::move(_buffer));
        _is_buffer_empty = false;
        _buffer.clear();
        _buffer.reserve(_chunk_size);
    }
    _empty_cv.notify_one();
}

void InternalNode::process() {
    std::vector<int32_t> left;
    std::vector<int32_t> right;
    bool l_need_more = true;
    bool r_need_more = true;
    int32_t li = 0;
    int32_t ri = 0;
    while (!_l_child->eos() || !_r_child->eos() || li < left.size() || ri < right.size()) {
        if (l_need_more) {
            while (_l_child->has_more_intput() && _l_child->is_buffer_empty()) {
                std::unique_lock<std::mutex> l(_l_child->_m);
                if (_l_child->has_more_intput() && _l_child->is_buffer_empty()) {
                    _l_child->_empty_cv.wait(l);
                }
            }
            if (!_l_child->is_buffer_empty()) {
                _l_child->fetch_from_buffer(left);
            }
            l_need_more = false;
        }
        if (r_need_more) {
            while (_r_child->has_more_intput() && _r_child->is_buffer_empty()) {
                std::unique_lock<std::mutex> l(_r_child->_m);
                if (_r_child->has_more_intput() && _r_child->is_buffer_empty()) {
                    _r_child->_empty_cv.wait(l);
                }
            }
            if (!_r_child->is_buffer_empty()) {
                _r_child->fetch_from_buffer(right);
            }
            r_need_more = false;
        }

        _process(left, li, l_need_more, right, ri, r_need_more);

        // TODO memory will rise up in case of merge path
        if (li >= left.size()) {
            left.clear();
            li = 0;
            l_need_more = true;
        }

        if (ri >= right.size()) {
            right.clear();
            ri = 0;
            r_need_more = true;
        }
    }

    {
        std::unique_lock<std::mutex> l(_m);
        enqueue_buffer(l, true);
        _has_more_intput = false;
    }
}

void LeafNode::process() {
    int32_t i = 0;
    while (i < _nums.size()) {
        std::unique_lock<std::mutex> l(_m);
        int32_t buffer_remain_size = _chunk_size - _buffer.size();
        int32_t nums_remain_size = _nums.size() - i;
        int32_t step = std::min(buffer_remain_size, nums_remain_size);
        _buffer.insert(_buffer.end(), _nums.begin() + i, _nums.begin() + i + step);
        i += step;
        enqueue_buffer(l, false);
    }
    {
        std::unique_lock<std::mutex> l(_m);
        enqueue_buffer(l, true);
        _has_more_intput = false;
    }
}

StreamMerger::~StreamMerger() {
    for (int32_t i = 0; i < _threads.size(); ++i) {
        _threads[i].join();
    }
}
