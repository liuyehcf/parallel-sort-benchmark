#include <algorithm>
#include <iostream>
#include <random>

#include "merge_path_sorter.h"
#include "simple_merge_sorter.h"
#include "simple_quick_sorter.h"
#include "sorter.h"

void print(const std::vector<int32_t>& v) {
    for (size_t i = 0; i < v.size(); i++) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << v[i];
    }
    std::cout << std::endl;
}

void check(Sorter* sorter) {
    static constexpr int32_t max = 100;
    static std::default_random_engine e;
    static std::uniform_int_distribution<int32_t> u(1, max);

    static std::vector<size_t> lengths = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 1048576};

    static size_t max_processor_num = 16;

    for (const auto length : lengths) {
        for (size_t processor_num = 1; processor_num < max_processor_num; ++processor_num) {
            std::vector<int32_t> nums;
            std::vector<int32_t> expected_num;
            for (size_t i = 0; i < length; ++i) {
                auto num = u(e);
                nums.push_back(num);
                expected_num.push_back(num);
            }

            std::sort(expected_num.begin(), expected_num.end());
            sorter->sort(nums, processor_num);

            for (size_t i = 0; i < length; ++i) {
                if (nums[i] != expected_num[i]) {
                    throw std::logic_error("wrong");
                }
            }

            std::cout << "length=" << length << ", processor_num=" << processor_num << std::endl;
        }
    }
}

int main() {
    Sorter* simple_merge_sorter = new SimpleMergeSorter();
    check(simple_merge_sorter);

    Sorter* merge_path_sorter = new MergePathSorter();
    check(merge_path_sorter);

    Sorter* simple_quick_sorter = new SimpleQuickSorter();
    check(simple_quick_sorter);
    return 0;
}
