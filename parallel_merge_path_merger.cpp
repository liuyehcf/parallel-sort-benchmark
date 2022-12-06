#include "parallel_merge_path_merger.h"

#include "merge_path.h"
#include "util.h"

void ParallelMergePathInternalNode::_process(const std::vector<int32_t>& left, int32_t& li, bool& l_need_more,
                                             const std::vector<int32_t>& right, int32_t& ri, bool& r_need_more) {
    const bool l_has_more = !_l_child->eos();
    const bool r_has_more = !_r_child->eos();
    const bool l_lt_chunk_size = ((left.size() - li) < _chunk_size);
    const bool r_lt_chunk_size = ((right.size() - ri) < _chunk_size);
    int32_t l_size;
    int32_t r_size;
    if (l_lt_chunk_size || r_lt_chunk_size) {
        if (l_has_more || r_has_more) {
            if (l_has_more) {
                l_need_more = true;
            }
            if (r_has_more) {
                r_need_more = true;
            }
            return;
        }
        l_size = left.size() - li;
        r_size = right.size() - ri;
    } else {
        l_size = _chunk_size;
        r_size = _chunk_size;
    }

    std::vector<int32_t> merged;
    if (l_size == 0) {
        merged.insert(merged.end(), right.begin() + ri, right.begin() + ri + r_size);
        ri += r_size;
    } else if (r_size == 0) {
        merged.insert(merged.end(), left.begin() + li, left.begin() + li + l_size);
        li += l_size;
    } else {
        merged.resize(std::min(l_size, r_size));
        int32_t l_step;
        int32_t r_step;
        MergePath::merge(&left.data()[li], l_size, &l_step, &right.data()[ri], r_size, &r_step, merged.data(),
                         merged.size(), _processor_num);

        CHECK(l_step + r_step == merged.size());
        li += l_step;
        ri += r_step;
    }

    int32_t i = 0;
    while (i < merged.size()) {
        std::unique_lock<std::mutex> l(_m);
        const int32_t remain_size = merged.size() - i;
        const int32_t buffer_remain_size = _chunk_size - _buffer.size();
        const int32_t step = std::min(remain_size, buffer_remain_size);

        _buffer.insert(_buffer.end(), merged.begin() + i, merged.begin() + i + step);
        i += step;
        enqueue_buffer(l, false);
    }

    CHECK(i == merged.size());
}

std::vector<int32_t> ParallelMergePathMerger::pull() {
    std::vector<int32_t> out;
    if (!_root->is_buffer_empty()) {
        _root->fetch_from_buffer(out);
    }
    if (_root->eos()) {
        _eos = true;
    }
    return out;
};

void ParallelMergePathMerger::_init() {
    std::vector<NodePtr> current_level;
    for (const auto& nums : _multi_nums) {
        NodePtr node = std::make_shared<LeafNode>(_chunk_size, _max_buffer_size, nums);
        _start(node);
        current_level.emplace_back(node);
    }

    while (current_level.size() > 1) {
        std::vector<NodePtr> next_level;
        int32_t avg_processor_num = _processor_num / current_level.size();
        if (avg_processor_num <= 0) {
            avg_processor_num = 1;
        }

        int32_t i = 0;
        for (; i + 1 < current_level.size(); i += 2) {
            const auto& left = current_level[i];
            const auto& right = current_level[i + 1];

            NodePtr node = std::make_shared<ParallelMergePathInternalNode>(_chunk_size, _max_buffer_size, left, right,
                                                                           avg_processor_num);
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

void ParallelMergePathMerger::_start(const NodePtr& node) {
    _threads.emplace_back([node]() { node->process(); });
}

int test_parallel_merge_path_merger() {
    std::vector<std::vector<int32_t>> multi_nums{
            {696}, {36, 55, 393, 532, 851, 958}, {8, 69, 393, 428, 688}, {95, 540, 604, 670, 867, 953}};
    ParallelMergePathMerger merger(multi_nums, 1, 6, 1);

    while (!merger.eos()) {
        auto&& nums = merger.pull();
        if (!nums.empty()) {
            print(nums);
        }
    }
    return 0;
}
