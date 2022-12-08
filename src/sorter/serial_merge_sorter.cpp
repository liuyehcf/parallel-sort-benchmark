#include "serial_merge_sorter.h"

#include <algorithm>
#include <thread>

#include "util.h"

void SerialMergeSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    std::vector<std::vector<int32_t>> current_level = _split_and_sort(nums, processor_num);
    while (current_level.size() > 1) {
        std::vector<std::vector<int32_t>> next_level;

        const auto current_size = current_level.size();

        int32_t i = 0;
        for (; i + 1 < current_size; i += 2) {
            auto& left = current_level[i];
            auto& right = current_level[i + 1];
            std::vector<int32_t> merged;
            merged.resize(left.size() + right.size());

            std::merge(left.begin(), left.end(), right.begin(), right.end(), merged.begin());

            next_level.emplace_back(std::move(merged));
        }

        if (i < current_size) {
            next_level.emplace_back(std::move(current_level[i]));
        }

        current_level = std::move(next_level);
    }

    std::swap(nums, current_level[0]);
}
