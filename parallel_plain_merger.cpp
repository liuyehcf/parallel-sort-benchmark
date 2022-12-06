#include "parallel_plain_merger.h"

#include <mutex>

#include "util.h"

void ParallelPlainMergeInternalNode::_process(const std::vector<int32_t>& left, int32_t& li, bool& l_need_more,
                                              const std::vector<int32_t>& right, int32_t& ri, bool& r_need_more) {
    std::unique_lock<std::mutex> l(_m);
    while (li < left.size() && ri < right.size()) {
        if (left[li] <= right[ri]) {
            _buffer.push_back(left[li]);
            li++;
        } else {
            _buffer.push_back(right[ri]);
            ri++;
        }
        enqueue_buffer(l, false);
    }

    while (_r_child->eos() && li < left.size()) {
        _buffer.push_back(left[li]);
        li++;
        enqueue_buffer(l, false);
    }

    while (_l_child->eos() && ri < right.size()) {
        _buffer.push_back(right[ri]);
        ri++;
        enqueue_buffer(l, false);
    }
}

std::vector<int32_t> ParallelPlainMerger::pull() {
    std::vector<int32_t> out;
    if (!_root->is_buffer_empty()) {
        _root->fetch_from_buffer(out);
    }
    if (_root->eos()) {
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

            NodePtr node = std::make_shared<ParallelPlainMergeInternalNode>(_chunk_size, _max_buffer_size, left, right);
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

int test_parallel_plain_merger() {
    std::vector<int32_t> v1{17, 24, 28, 28, 36, 44, 48, 49, 77, 90};
    std::vector<int32_t> v2{7, 10, 27, 32, 50, 51, 52, 91, 95, 99};

    std::vector<std::vector<int32_t>> multi_nums{v1, v2};
    ParallelPlainMerger merger(multi_nums, -1, 3, 1);

    while (!merger.eos()) {
        auto&& nums = merger.pull();
        if (nums.empty()) {
            print(nums);
        }
    }

    return 0;
}
