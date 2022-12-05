#include <algorithm>
#include <iostream>
#include <random>

#include "blocking_sorter.h"
#include "parallel_block_based_quick_sorter.h"
#include "parallel_merge_path_sorter.h"
#include "parallel_plain_merge_sorter.h"
#include "parallel_plain_merger.h"
#include "parallel_plain_quick_sorter.h"
#include "serial_k_merger.h"
#include "streaming_merger.h"
#include "util.h"

void check(BlockingSorter* sorter) {
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
                CHECK(nums[i] == expected_nums[i]);
            }

            std::cout << "length=" << length << ", processor_num=" << processor_num << std::endl;
        }
    }
}

template <typename T>
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

        std::cout << "chunk_size=" << chunk_size << ", total_size=" << expected_nums.size()
                  << ", multi_nums_size=" << multi_nums.size() << std::endl;

        T merger(std::move(multi_nums), chunk_size);
        std::vector<int32_t> merged_nums;
        while (!merger.eos()) {
            std::vector<int32_t> chunk = merger.pull();
            if (!chunk.empty()) {
                merged_nums.insert(merged_nums.end(), chunk.begin(), chunk.end());
            }
        }

        std::sort(expected_nums.begin(), expected_nums.end());

        CHECK(merged_nums.size() == expected_nums.size());
        for (int32_t i = 0; i < expected_nums.size(); ++i) {
            CHECK(merged_nums[i] == expected_nums[i]);
        }
    }
}

int main() {
    BlockingSorter* simple_merge_sorter = new ParallelPlainMergeSorter();
    check(simple_merge_sorter);
    delete simple_merge_sorter;

    BlockingSorter* merge_path_sorter = new ParallelMergePathSorter();
    check(merge_path_sorter);
    delete merge_path_sorter;

    BlockingSorter* simple_quick_sorter = new ParallelPlainQuickSorter();
    check(simple_quick_sorter);
    delete simple_quick_sorter;

    BlockingSorter* block_quick_sorter = new ParallelBlockBasedQuickSorter(false, 1024);
    check(block_quick_sorter);
    delete block_quick_sorter;

    check_stream_k_merger<SerialKMerger>();
    check_stream_k_merger<ParallelPlainMerger>();
    return 0;
}
