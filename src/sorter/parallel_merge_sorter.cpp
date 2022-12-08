#include "parallel_merge_sorter.h"

#include <algorithm>
#include <mutex>
#include <thread>

#include "util.h"

void ParallelMergeSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    std::vector<std::vector<int32_t>> current_level = _split_and_sort(nums, processor_num);
    std::mutex m;

    while (current_level.size() > 1) {
        std::vector<std::vector<int32_t>> next_level;

        const auto current_size = current_level.size();
        const auto avg_processor_num = processor_num / (current_size / 2);
        CHECK(avg_processor_num >= 1);

        int32_t i = 0;
        std::vector<std::thread> merge_threads;
        for (; i + 1 < current_size; i += 2) {
            merge_threads.emplace_back([this, &m, &current_level, &next_level, avg_processor_num, i]() {
                auto& left = current_level[i];
                auto& right = current_level[i + 1];
                std::vector<int32_t> merged;
                merged.resize(left.size() + right.size());

                _merge(left, right, merged, avg_processor_num);

                std::lock_guard<std::mutex> l(m);
                next_level.emplace_back(std::move(merged));
            });
        }
        for (int32_t i = 0; i < merge_threads.size(); ++i) {
            merge_threads[i].join();
        }

        if (i < current_size) {
            next_level.emplace_back(std::move(current_level[i]));
        }

        current_level = std::move(next_level);
    }

    std::swap(nums, current_level[0]);
}
