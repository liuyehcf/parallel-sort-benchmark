#include "simple_merge_sorter.h"

#include <algorithm>
#include <mutex>
#include <thread>

void SimpleMergeSorter::sort(std::vector<int32_t>& nums, const size_t processor_num) {
    std::vector<std::vector<int32_t>> partial_nums;
    partial_nums.resize(processor_num);

    for (size_t i = 0; i < processor_num; ++i) {
        partial_nums[i].reserve(nums.size() / processor_num);
    }

    for (size_t i = 0; i < nums.size(); ++i) {
        partial_nums[i % processor_num].push_back(nums[i]);
    }

    for (size_t i = 0; i < processor_num; ++i) {
        std::sort(partial_nums[i].begin(), partial_nums[i].end());
    }

    std::mutex m;
    std::vector<std::vector<int32_t>> current_level = std::move(partial_nums);

    while (current_level.size() > 1) {
        std::vector<std::vector<int32_t>> next_level;

        const auto current_size = current_level.size();

        size_t i = 0;
        std::vector<std::thread> threads;
        for (; i + 1 < current_size; i += 2) {
            threads.emplace_back([this, &m, &current_level, &next_level, i]() {
                auto& left = current_level[i];
                auto& right = current_level[i + 1];
                std::vector<int32_t> merged;
                merged.resize(left.size() + right.size());

                _merge(left, right, merged);

                std::lock_guard<std::mutex> l(m);
                next_level.emplace_back(std::move(merged));
            });
        }
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }

        if (i < current_size) {
            next_level.emplace_back(std::move(current_level[i]));
        }

        current_level = std::move(next_level);
    }

    std::swap(nums, current_level[0]);
}

void SimpleMergeSorter::_merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right,
                               std::vector<int32_t>& dest) {
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            dest[k] = left[i];
            k++;
            i++;
        } else {
            dest[k] = right[j];
            k++;
            j++;
        }
    }

    while (i < left.size()) {
        dest[k] = left[i];
        k++;
        i++;
    }

    while (j < right.size()) {
        dest[k] = right[j];
        k++;
        j++;
    }
}
