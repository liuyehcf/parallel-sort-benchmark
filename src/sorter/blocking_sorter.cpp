#include "blocking_sorter.h"

#include <algorithm>
#include <thread>

std::vector<std::vector<int32_t>> BlockingSorter::_split_and_sort(const std::vector<int32_t>& nums,
                                                                  const int32_t processor_num) {
    std::vector<std::vector<int32_t>> multi_nums;
    multi_nums.resize(processor_num);
    const int32_t avg_size = nums.size() / processor_num;

    std::vector<std::thread> threads;
    for (int32_t i = 0; i < processor_num; ++i) {
        threads.emplace_back([&nums, &multi_nums, avg_size, processor_num, i]() {
            int32_t start = avg_size * i;
            int32_t end = avg_size * (i + 1);
            if (i == processor_num - 1) {
                end = nums.size();
            }
            multi_nums[i].reserve(avg_size);
            std::copy(nums.begin() + start, nums.begin() + end, std::back_inserter(multi_nums[i]));
            std::sort(multi_nums[i].begin(), multi_nums[i].end());
        });
    }
    for (int32_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }

    return multi_nums;
}
