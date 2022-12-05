#include "parallel_plain_merger.h"

#include <mutex>

#include "util.h"

void Node::fetch_from_buffer(std::vector<int32_t>& output) {
    std::lock_guard<std::mutex> l(_m);
    CHECK(!_buffers.empty());
    output.swap(_buffers.front());
    _buffers.pop();
    _full_cv.notify_one();
}

void InternalNode::process() {
    std::vector<int32_t> left_input;
    std::vector<int32_t> right_input;
    bool left_need_input = true;
    bool right_need_input = true;
    int32_t li = 0;
    int32_t ri = 0;
    while (!_left->eos() || !_right->eos() || !_left->is_buffer_empty() || !_right->is_buffer_empty()) {
        if (left_need_input) {
            while (!_left->eos() && _left->is_buffer_empty()) {
                std::unique_lock<std::mutex> l(_left->_m);
                if (!_left->eos() && _left->is_buffer_empty()) {
                    _left->_empty_cv.wait(l);
                }
            }
            if (!_left->is_buffer_empty()) {
                _left->fetch_from_buffer(left_input);
                li = 0;
            }
            left_need_input = false;
        }
        if (right_need_input) {
            while (!_right->eos() && _right->is_buffer_empty()) {
                std::unique_lock<std::mutex> l(_right->_m);
                if (!_right->eos() && _right->is_buffer_empty()) {
                    _right->_empty_cv.wait(l);
                }
            }
            if (!_right->is_buffer_empty()) {
                _right->fetch_from_buffer(right_input);
                ri = 0;
            }
            right_need_input = false;
        }

        {
            std::unique_lock<std::mutex> l(_m);
            while (li < left_input.size() && ri < right_input.size()) {
                if (left_input[li] <= right_input[ri]) {
                    _buffer.push_back(left_input[li]);
                    li++;
                } else {
                    _buffer.push_back(right_input[ri]);
                    ri++;
                }
                _buffer_enqueue(l, false);
            }

            while (_right->eos() && _right->is_buffer_empty() && li < left_input.size()) {
                _buffer.push_back(left_input[li]);
                li++;
                _buffer_enqueue(l, false);
            }

            while (_left->eos() && _left->is_buffer_empty() && ri < right_input.size()) {
                _buffer.push_back(right_input[ri]);
                ri++;
                _buffer_enqueue(l, false);
            }
        }

        if (li >= left_input.size()) {
            left_need_input = true;
        }

        if (ri >= right_input.size()) {
            right_need_input = true;
        }
    }

    {
        std::unique_lock<std::mutex> l(_m);
        _buffer_enqueue(l, true);
    }
    _eos = true;
}

void Node::_buffer_enqueue(std::unique_lock<std::mutex>& l, bool force) {
    if (!force && _buffer.size() < _chunk_size) {
        return;
    }
    if (!_buffer.empty()) {
        if (_buffers.size() >= _max_buffer_size) {
            _full_cv.wait(l);
        }
        CHECK(_buffers.size() < _max_buffer_size);
        _buffers.push(std::move(_buffer));
        _buffer.clear();
        _buffer.reserve(_chunk_size);
    }
    _empty_cv.notify_one();
}

void LeafNode::process() {
    int32_t i = 0;
    while (i < _nums.size()) {
        std::unique_lock<std::mutex> l(_m);
        int32_t buffer_remain_size = _max_buffer_size - _buffer.size();
        int32_t nums_remain_size = _nums.size() - i;
        int32_t step = std::min(buffer_remain_size, nums_remain_size);
        _buffer.insert(_buffer.end(), _nums.begin() + i, _nums.begin() + i + step);
        i += step;
        _buffer_enqueue(l, false);
    }
    {
        std::unique_lock<std::mutex> l(_m);
        _buffer_enqueue(l, true);
    }
    _eos = true;
}

ParallelPlainMerger::~ParallelPlainMerger() {
    for (int32_t i = 0; i < _threads.size(); ++i) {
        _threads[i].join();
    }
}

std::vector<int32_t> ParallelPlainMerger::pull() {
    std::vector<int32_t> out;
    if (!_root->is_buffer_empty()) {
        _root->fetch_from_buffer(out);
    }
    if (_root->eos() && _root->is_buffer_empty()) {
        _eos = true;
    }
    return out;
}

void ParallelPlainMerger::_init() {
    std::vector<NodePtr> current_level;
    for (const auto& nums : _multi_nums) {
        NodePtr node = std::make_shared<LeafNode>(_chunk_size, _max_buffer_size, nums);
        _start(node);
        current_level.emplace_back(node);
    }

    while (current_level.size() > 1) {
        std::vector<NodePtr> next_level;

        int32_t i = 0;
        for (; i + 1 < current_level.size(); i += 2) {
            const auto& left = current_level[i];
            const auto& right = current_level[i + 1];

            NodePtr node = std::make_shared<InternalNode>(_chunk_size, _max_buffer_size, left, right);
            _start(node);
            next_level.emplace_back(node);
        }

        if (i < current_level.size()) {
            next_level.emplace_back(std::move(current_level[current_level.size() - 1]));
        }

        next_level.swap(current_level);
    }

    _root = current_level[0];
}

void ParallelPlainMerger::_start(const NodePtr& node) {
    _threads.emplace_back([node]() { node->process(); });
}

int test_simple_merge_merger() {
    std::vector<int32_t> v1{17, 24, 28, 28, 36, 44, 48, 49, 77, 90};
    std::vector<int32_t> v2{7, 10, 27, 32, 50, 51, 52, 91, 95, 99};

    std::vector<std::vector<int32_t>> multi_nums{v1, v2};
    ParallelPlainMerger merger(std::move(multi_nums), 3, 1);

    while (!merger.eos()) {
        auto&& nums = merger.pull();
        if (nums.empty()) {
            print(nums);
        }
    }

    return 0;
}
