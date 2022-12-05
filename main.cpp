#include <algorithm>
#include <iostream>
#include <random>

#include "block_quick_sorter.h"
#include "k_merger.h"
#include "merge_path_sorter.h"
#include "merger.h"
#include "simple_merge_sorter.h"
#include "simple_quick_sorter.h"
#include "sorter.h"
#include "util.h"

void check(Sorter* sorter) {
    static constexpr int32_t max = 100;
    static std::default_random_engine e;
    static std::uniform_int_distribution<int32_t> u(1, max);

    static std::vector<int32_t> lengths = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 1048576};

    static int32_t max_processor_num = 16;

    for (const auto length : lengths) {
        for (int32_t processor_num = 1; processor_num < max_processor_num; ++processor_num) {
            std::vector<int32_t> nums;
            std::vector<int32_t> expected_nums;
            for (int32_t i = 0; i < length; ++i) {
                auto num = u(e);
                nums.push_back(num);
                expected_nums.push_back(num);
            }

            std::sort(expected_nums.begin(), expected_nums.end());
            sorter->sort(nums, processor_num);

            for (int32_t i = 0; i < length; ++i) {
                if (nums[i] != expected_nums[i]) {
                    throw std::logic_error("wrong");
                }
            }

            std::cout << "length=" << length << ", processor_num=" << processor_num << std::endl;
        }
    }
}

void check_stream_k_merger() {
    static std::default_random_engine e;
    static std::uniform_int_distribution<int32_t> u(1, 100);
    static std::uniform_int_distribution<int32_t> nums_size_u(64, 1024);
    static std::uniform_int_distribution<int32_t> multi_nums_size_u(1, 16);

    for (int32_t chunk_size = 1; chunk_size < 64; ++chunk_size) {
        std::vector<std::vector<int32_t>> multi_nums;
        std::vector<int32_t> expected_nums;
        int32_t multi_nums_size = multi_nums_size_u(e);
        for (int32_t i = 0; i < multi_nums_size; i++) {
            multi_nums.emplace_back();
            int32_t nums_size = nums_size_u(e);
            for (int32_t j = 0; j < nums_size; j++) {
                auto num = u(e);
                multi_nums[i].push_back(num);
                expected_nums.push_back(num);
            }
            std::sort(multi_nums[i].begin(), multi_nums[i].end());
        }

        std::cout << "chunk_size=" << chunk_size << ", total_size=" << expected_nums.size() << std::endl;

        KMerger merger(std::move(multi_nums), chunk_size);
        std::vector<int32_t> merged_nums;
        while (!merger.eos()) {
            std::vector<int32_t> chunk = merger.pull();
            if (!chunk.empty()) {
                merged_nums.insert(merged_nums.end(), chunk.begin(), chunk.end());
            }
        }

        std::sort(expected_nums.begin(), expected_nums.end());

        if (merged_nums.size() != expected_nums.size()) {
            throw std::logic_error("wrong");
        }
        for (int32_t i = 0; i < expected_nums.size(); ++i) {
            if (merged_nums[i] != expected_nums[i]) {
                throw std::logic_error("wrong");
            }
        }
    }
}

int main() {
    Sorter* simple_merge_sorter = new SimpleMergeSorter();
    check(simple_merge_sorter);
    delete simple_merge_sorter;

    Sorter* merge_path_sorter = new MergePathSorter();
    check(merge_path_sorter);
    delete merge_path_sorter;

    Sorter* simple_quick_sorter = new SimpleQuickSorter();
    check(simple_quick_sorter);
    delete simple_quick_sorter;

    Sorter* block_quick_sorter = new BlockBasedQuickSorter(false, 1024);
    check(block_quick_sorter);
    delete block_quick_sorter;

    check_stream_k_merger();
    return 0;
}
