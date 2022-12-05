#include "parallel_plain_quick_sorter.h"

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "util.h"

struct Slice {
    Slice(std::vector<int32_t>& nums, const int32_t start, const int32_t end) : nums(nums), start(start), end(end){};
    std::vector<int32_t>& nums;
    const int32_t start;
    const int32_t end;
};

void ParallelPlainQuickSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    std::vector<Slice> current_level;
    current_level.emplace_back(Slice(nums, 0, nums.size()));
    while (current_level.size() < processor_num) {
        std::vector<Slice> next_level;
        const auto current_size = current_level.size();
        const auto avg_processor_num = processor_num / current_size;
        CHECK(avg_processor_num >= 1);

        std::mutex m;

        std::vector<std::thread> threads;
        int32_t next_level_size = 0;
        for (int32_t i = 0; i < current_size; i++) {
            Slice& slice = current_level[i];
            threads.emplace_back([this, &m, &next_level, &slice]() {
                int32_t mid = _partition(slice.nums, slice.start, slice.end);

                std::lock_guard<std::mutex> l(m);
                next_level.emplace_back(Slice(slice.nums, slice.start, mid));
                next_level.emplace_back(Slice(slice.nums, mid, slice.end));
            });
            next_level_size += 2;
            const int32_t current_remain_size = current_size - (i + 1);
            if (next_level_size + current_remain_size >= processor_num) {
                std::lock_guard<std::mutex> l(m);
                for (; i + 1 < current_size; ++i) {
                    next_level.emplace_back(std::move(current_level[i + 1]));
                }
                break;
            }
        }

        for (int32_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }

        std::swap(current_level, next_level);
    }

    CHECK(current_level.size() == processor_num);

    std::vector<std::thread> threads;
    for (int32_t i = 0; i < current_level.size(); ++i) {
        threads.emplace_back([&current_level, i]() {
            Slice& slice = current_level[i];
            if (slice.start < slice.end) {
                std::sort(slice.nums.begin() + slice.start, slice.nums.begin() + slice.end);
            }
        });
    }

    for (int32_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
}

int32_t ParallelPlainQuickSorter::_partition(std::vector<int32_t>& nums, int32_t start, int32_t end) {
    if (start >= end) {
        return start;
    }
    const int32_t pivot = nums[end - 1];
    int32_t i = start - 1;

    for (int32_t j = start; j < end - 1; j++) {
        if (nums[j] < pivot) {
            _swap(nums, ++i, j);
        }
    }

    _swap(nums, ++i, end - 1);

    return i;
}

void ParallelPlainQuickSorter::_swap(std::vector<int32_t>& nums, int32_t i, int32_t j) {
    if (i == j) {
        return;
    }
    int32_t tmp = nums[i];
    nums[i] = nums[j];
    nums[j] = tmp;
}
