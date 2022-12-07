#include <algorithm>
#include <iostream>
#include <random>

#include "blocking_sorter.h"
#include "merge_path.h"
#include "parallel_block_based_quick_sorter.h"
#include "parallel_merge_path_merger.h"
#include "parallel_merge_path_sorter.h"
#include "parallel_plain_merge_sorter.h"
#include "parallel_plain_merger.h"
#include "parallel_plain_quick_sorter.h"
#include "serial_k_merger.h"
#include "serial_merge_sorter.h"
#include "streaming_merger.h"
#include "util.h"

void check_merge_path() {
    static std::default_random_engine E;
    static std::uniform_int_distribution<int32_t> VALUE_U(1, 100);
    static std::uniform_int_distribution<int32_t> LENGTH_U(1, 100);
    static std::uniform_int_distribution<int32_t> PROCESSOR_NUM_U(1, 16);
    static int32_t COUNT = 16;

    for (int32_t cnt = 0; cnt < COUNT; ++cnt) {
        std::vector<int32_t> left;
        std::vector<int32_t> right;
        left.resize(LENGTH_U(E));
        right.resize(LENGTH_U(E));

        std::vector<int32_t> expected_nums;
        for (int32_t i = 0; i < left.size(); i++) {
            const auto value = VALUE_U(E);
            left[i] = value;
            expected_nums.push_back(value);
        }
        for (int32_t i = 0; i < right.size(); i++) {
            const auto value = VALUE_U(E);
            right[i] = value;
            expected_nums.push_back(value);
        }
        std::sort(left.begin(), left.end());
        std::sort(right.begin(), right.end());
        std::sort(expected_nums.begin(), expected_nums.end());

        for (int32_t l_len = 0; l_len <= left.size(); l_len++) {
            for (int32_t r_len = 0; r_len <= right.size(); r_len++) {
                if (l_len == 0 || r_len == 0) {
                    continue;
                }
                std::cout << "cnt=" << cnt << ", l_len=" << l_len << ", r_len=" << r_len << std::endl;
                std::vector<int32_t> dest;
                dest.resize(l_len + r_len);
                int32_t l_step;
                int32_t r_step;
                MergePath::merge(left.data(), l_len, &l_step, right.data(), r_len, &r_step, dest.data(), dest.size(),
                                 PROCESSOR_NUM_U(E));
                CHECK(l_step + r_step == dest.size());
                for (int32_t i = 0; i < std::min(l_len, r_len); i++) {
                    CHECK(dest[i] == expected_nums[i]);
                }
            }
        }
        std::vector<int32_t> dest;
        dest.resize(left.size() + right.size());
        int32_t l_step;
        int32_t r_step;
        MergePath::merge(left.data(), left.size(), &l_step, right.data(), right.size(), &r_step, dest.data(),
                         dest.size(), PROCESSOR_NUM_U(E));
        CHECK(left.size() + right.size() == dest.size());
        for (int32_t i = 0; i < dest.size(); i++) {
            CHECK(dest[i] == expected_nums[i]);
        }
    }
}

void check_blocking_sorter(BlockingSorter* sorter) {
    static std::default_random_engine E;
    static std::uniform_int_distribution<int32_t> VALUE_U(1, 100);
    static std::vector<int32_t> lengths = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 1048576};
    static int32_t MAX_PROCESSOR_NUM = 16;

    for (const auto length : lengths) {
        for (int32_t processor_num = 1; processor_num < MAX_PROCESSOR_NUM; ++processor_num) {
            std::vector<int32_t> nums;
            std::vector<int32_t> expected_nums;
            for (int32_t i = 0; i < length; ++i) {
                auto num = VALUE_U(E);
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

template <typename T, bool test_processor_num, bool test_max_buffer_size>
void check_stream_merger() {
    static std::default_random_engine E;
    static std::uniform_int_distribution<int32_t> VALUE_U(1, 1024);
    static std::uniform_int_distribution<int32_t> NUMS_SIZE_U(1, 1024);
    static std::uniform_int_distribution<int32_t> MULTI_NUMS_SIZE_U(1, 16);
    static std::uniform_int_distribution<int32_t> INC_U(1, 8);
    static int32_t MAX_PROCESSOR_NUM = 16;
    static int32_t MAX_CHUNK_SIZE = 256;
    static int32_t MAX_MAX_BUFFER_SIZE = 16;

    for (int32_t processor_num = (test_processor_num ? 1 : MAX_PROCESSOR_NUM); processor_num <= MAX_PROCESSOR_NUM;
         processor_num++) {
        for (int32_t max_buffer_size = (test_max_buffer_size ? 1 : MAX_MAX_BUFFER_SIZE);
             max_buffer_size <= MAX_MAX_BUFFER_SIZE; max_buffer_size++) {
            for (int32_t chunk_size = 1; chunk_size < MAX_CHUNK_SIZE; chunk_size += INC_U(E)) {
                std::vector<std::vector<int32_t>> multi_nums;
                std::vector<int32_t> expected_nums;
                int32_t multi_nums_size = MULTI_NUMS_SIZE_U(E);
                for (int32_t i = 0; i < multi_nums_size; i++) {
                    multi_nums.emplace_back();
                    int32_t nums_size = NUMS_SIZE_U(E);
                    for (int32_t j = 0; j < nums_size; j++) {
                        auto num = VALUE_U(E);
                        multi_nums[i].push_back(num);
                        expected_nums.push_back(num);
                    }
                    std::sort(multi_nums[i].begin(), multi_nums[i].end());
                }

                std::cout << typeid(T).name() << ", processor_num=" << processor_num << ", chunk_size=" << chunk_size
                          << ", max_buffer_size=" << max_buffer_size << ", total_size=" << expected_nums.size()
                          << ", multi_nums_size=" << multi_nums.size() << std::endl;

                T merger(multi_nums, processor_num, chunk_size, max_buffer_size);
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
                    if (merged_nums[i] != expected_nums[i]) {
                        PRINT(merged_nums);
                        PRINT(expected_nums);
                        for (auto& nums : multi_nums) {
                            PRINT(nums);
                        }
                        exit(1);
                    }
                }
            }
        }
    }
}

int main() {
    check_merge_path();

    SerialMergeSorter* serial_merge_sorter = new SerialMergeSorter();
    check_blocking_sorter(serial_merge_sorter);
    delete serial_merge_sorter;

    BlockingSorter* simple_merge_sorter = new ParallelPlainMergeSorter();
    check_blocking_sorter(simple_merge_sorter);
    delete simple_merge_sorter;

    BlockingSorter* merge_path_sorter = new ParallelMergePathSorter();
    check_blocking_sorter(merge_path_sorter);
    delete merge_path_sorter;

    BlockingSorter* simple_quick_sorter = new ParallelPlainQuickSorter();
    check_blocking_sorter(simple_quick_sorter);
    delete simple_quick_sorter;

    BlockingSorter* block_quick_sorter = new ParallelBlockBasedQuickSorter(false, 1024);
    check_blocking_sorter(block_quick_sorter);
    delete block_quick_sorter;

    check_stream_merger<SerialKMerger, false, false>();
    check_stream_merger<ParallelPlainMerger, false, true>();
    check_stream_merger<ParallelMergePathMerger, true, true>();
    return 0;
}
