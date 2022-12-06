#include "parallel_merge_path_sorter.h"

#include <algorithm>
#include <exception>
#include <mutex>
#include <thread>

#include "merge_path.h"
#include "util.h"

void ParallelMergePathSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    std::vector<std::vector<int32_t>> partial_nums;
    partial_nums.resize(processor_num);

    for (int32_t i = 0; i < processor_num; ++i) {
        partial_nums[i].reserve(nums.size() / processor_num);
    }

    for (int32_t i = 0; i < nums.size(); ++i) {
        partial_nums[i % processor_num].push_back(nums[i]);
    }

    for (int32_t i = 0; i < processor_num; ++i) {
        std::sort(partial_nums[i].begin(), partial_nums[i].end());
    }

    std::mutex m;
    std::vector<std::vector<int32_t>> current_level = std::move(partial_nums);

    while (current_level.size() > 1) {
        std::vector<std::vector<int32_t>> next_level;

        const auto current_size = current_level.size();
        const auto avg_processor_num = processor_num / (current_size / 2);
        CHECK(avg_processor_num >= 1);

        int32_t i = 0;
        std::vector<std::thread> threads;
        for (; i + 1 < current_size; i += 2) {
            threads.emplace_back([&m, &current_level, &next_level, avg_processor_num, i]() {
                auto& left = current_level[i];
                auto& right = current_level[i + 1];
                std::vector<int32_t> merged;
                merged.resize(left.size() + right.size());

                MergePath::merge(left.data(), left.size(), nullptr, right.data(), right.size(), nullptr, merged.data(),
                                 merged.size(), avg_processor_num);

                std::lock_guard<std::mutex> l(m);
                next_level.emplace_back(std::move(merged));
            });
        }
        for (int32_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }

        if (i < current_size) {
            next_level.emplace_back(std::move(current_level[i]));
        }

        current_level = std::move(next_level);
    }

    std::swap(nums, current_level[0]);
}
