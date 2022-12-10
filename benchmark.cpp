#include <gflags/gflags.h>

#include <algorithm>
#include <iostream>
#include <limits>
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
#include "time_util.h"
#include "util.h"
#include "validator.h"

DEFINE_int32(mode, -1, "benchmark_mode, 1: parallel_plain_quick_sorter");
DEFINE_int32(data_size, 10000, "data_size");
DEFINE_int32(data_distribution, 1, "data_distribution, 1 for uniform; 2 for normal; 3 for skew");
DEFINE_int32(skew_percent, 95, "percentage of skew");
DEFINE_int32(min_dop, 1, "minimum degree of parallelism");
DEFINE_int32(max_dop, 1, "minimum degree of parallelism");
DEFINE_int32(block_size, 1024, "block size for block based quick sort");
DEFINE_int32(chunk_size, 4096, "chunk size of streaming merger");
DEFINE_int32(max_buffer_size, 256, "max buffer size of streaming merger");
DEFINE_int32(iteration, 3, "iteration times for each benchmark");
DEFINE_bool(merge_skew, false, "different stream has different data size");
DEFINE_int32(merge_skew_percent, 80, "how much percent of data does one skew parallelism holds");
DEFINE_bool(verbose, false, "verbose");

enum Mode {
    valid = 0,
    serial_merge_sorter = 1,
    parallel_plain_merge_sorter = 2,
    parallel_plain_quick_sorter = 3,
    parallel_merge_path_sorter = 4,
    parallel_block_based_quick_sorter = 5,
    serial_k_merger = 6,
    parallel_plain_merger = 7,
    parallel_merge_path_merger = 8,
};

enum Distribution {
    uniform = 1,
    normal = 2,
    skew = 3,
};

static std::default_random_engine e(0);
static std::uniform_int_distribution<int32_t> u(std::numeric_limits<int32_t>::min(),
                                                std::numeric_limits<int32_t>::max());
static std::uniform_int_distribution<int32_t> u_skew_majority(0, 10);
static std::uniform_int_distribution<int32_t> u_percent(0, 100);
static std::normal_distribution<double> n(0, 1024);

void check_flags();

template <typename T>
void benchmark_sorter();

template <typename T>
void benchmark_merger();

void init_nums(std::vector<int32_t>& nums, const int32_t size);

int main(int32_t argc, char* argv[]) {
    gflags::SetUsageMessage("Usage");
    gflags::SetVersionString("1.0.0");
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << gflags::CommandlineFlagsIntoString() << std::endl;

    check_flags();

    switch (FLAGS_mode) {
    case Mode::valid:
        validate();
        break;
    case serial_merge_sorter:
        benchmark_sorter<SerialMergeSorter>();
        break;
    case parallel_plain_merge_sorter:
        benchmark_sorter<ParallelPlainMergeSorter>();
        break;
    case parallel_plain_quick_sorter:
        benchmark_sorter<ParallelPlainQuickSorter>();
        break;
    case parallel_merge_path_sorter:
        benchmark_sorter<ParallelMergePathSorter>();
        break;
    case parallel_block_based_quick_sorter:
        ParallelBlockBasedQuickSorter::DEFAULT_BLOCK_SIZE = FLAGS_block_size;
        benchmark_sorter<ParallelBlockBasedQuickSorter>();
        break;
    case serial_k_merger:
        benchmark_merger<SerialKMerger>();
        break;
    case parallel_plain_merger:
        benchmark_merger<ParallelPlainMerger>();
        break;
    case parallel_merge_path_merger:
        benchmark_merger<ParallelMergePathMerger>();
        break;
    }

    return 0;
}

void check_flags() {
    if (FLAGS_mode < Mode::valid || FLAGS_mode > Mode::parallel_merge_path_merger) {
        std::cout << "mode must between " << Mode::valid << " and " << Mode::parallel_merge_path_merger << std::endl;
        exit(1);
    }
    if (FLAGS_data_size <= 0) {
        std::cout << "data_size must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_data_distribution < Distribution::uniform || FLAGS_data_distribution > Distribution::skew) {
        std::cout << "data_distribution must between " << Distribution::uniform << " and " << Distribution::skew
                  << std::endl;
        exit(1);
    }
    if (FLAGS_skew_percent < 0 || FLAGS_skew_percent > 100) {
        std::cout << "skew_percent must between 0 and 100" << std::endl;
        exit(1);
    }
    if (FLAGS_min_dop <= 0) {
        std::cout << "min_dop must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_mode >= Mode::serial_k_merger && FLAGS_min_dop <= 1) {
        std::cout << "min_dop for merger must greater than 1" << std::endl;
        exit(1);
    }
    if (FLAGS_max_dop <= 0) {
        std::cout << "max_dop must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_min_dop > FLAGS_max_dop) {
        std::cout << "min_dop must not greater than max_dop" << std::endl;
        exit(1);
    }
    if (FLAGS_block_size <= 0) {
        std::cout << "block_size must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_chunk_size <= 0) {
        std::cout << "chunk_size must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_max_buffer_size <= 0) {
        std::cout << "max_buffer_size must greater than 0" << std::endl;
        exit(1);
    }
    if (FLAGS_merge_skew_percent < 0 || FLAGS_merge_skew_percent > 100) {
        std::cout << "FLAGS_merge_skew_percent must between 0 and 100" << std::endl;
        exit(1);
    }
    if (FLAGS_iteration <= 0) {
        std::cout << "iteration must greater than 0" << std::endl;
        exit(1);
    }
}

template <typename T>
void benchmark_sorter() {
    std::vector<int32_t> nums;
    nums.resize(FLAGS_data_size);

    std::cout << "benchmark start, sorter=" << T::name << std::endl;
    std::vector<int64_t> avg_times;
    std::vector<int64_t> min_times;
    std::vector<int64_t> max_times;

    for (int32_t dop = FLAGS_min_dop; dop <= FLAGS_max_dop; dop++) {
        std::cout << " -> dop=" << dop << std::endl;
        int64_t avg_time = 0;
        int64_t min_time = std::numeric_limits<int64_t>::max();
        int64_t max_time = std::numeric_limits<int64_t>::min();
        for (int32_t cnt = 1; cnt <= FLAGS_iteration; ++cnt) {
            init_nums(nums, FLAGS_data_size);

            T sorter;
            int64_t time = 0;
            {
                SCOPED_TIMER(time);
                sorter.sort(nums, dop);
            }
            if (time < min_time) {
                min_time = time;
            }
            if (time > max_time) {
                max_time = time;
            }
            avg_time += time;
            if (FLAGS_verbose) {
                std::cout << "\titeration: " << cnt << "/" << FLAGS_iteration
                          << ", time usage: " << time_to_string(time) << std::endl;
            } else {
                if (cnt == 1) {
                    std::cout << "\titeration: " << cnt << "/" << FLAGS_iteration << std::flush;
                } else {
                    std::cout << ", " << cnt << "/" << FLAGS_iteration << std::flush;
                }
                if (cnt == FLAGS_iteration) {
                    std::cout << std::endl;
                }
            }
        }
        avg_time = avg_time / FLAGS_iteration;
        std::cout << "\taverage time usage: " << time_to_string(avg_time) << std::endl;
        std::cout << "\tmin time usage: " << time_to_string(min_time) << std::endl;
        std::cout << "\tmax time usage: " << time_to_string(max_time) << std::endl;

        avg_times.push_back(avg_time);
        min_times.push_back(min_time);
        max_times.push_back(max_time);
    }

    auto print = [](const std::string& text, const std::string& time_unit, const std::vector<int64_t> v) {
        std::cout << text << ": ";
        for (int32_t i = 0; i < v.size(); i++) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << time_to_string(v[i], time_unit);
        }
        std::cout << std::endl;
    };

    std::cout << "overall results:" << std::endl;
    print(" -> avg_times", "s", avg_times);
    print(" -> min_times", "s", min_times);
    print(" -> max_times", "s", max_times);
}

template <typename T>
void benchmark_merger() {
    std::cout << "benchmark start, merger=" << T::name << std::endl;
    std::vector<int64_t> avg_times;
    std::vector<int64_t> min_times;
    std::vector<int64_t> max_times;

    for (int32_t dop = FLAGS_min_dop; dop <= FLAGS_max_dop; dop++) {
        std::cout << " -> dop=" << dop << std::endl;
        int64_t avg_time = 0;
        int64_t min_time = std::numeric_limits<int64_t>::max();
        int64_t max_time = std::numeric_limits<int64_t>::min();

        std::vector<std::vector<int32_t>> multi_nums;
        multi_nums.resize(dop);
        if (FLAGS_merge_skew) {
            // One parallelism holds 80% of data, while the other parallelisms together
            // hold the remains
            const int32_t majority_size = static_cast<int32_t>(FLAGS_data_size / 100 * FLAGS_merge_skew_percent);
            const int32_t minority_size = FLAGS_data_size - majority_size;
            const int32_t avg_minority_size = minority_size / (dop - 1);
            std::vector<std::thread> init_threads;
            for (int i = 0; i < dop; ++i) {
                init_threads.emplace_back([&multi_nums, i, dop, majority_size, minority_size, avg_minority_size]() {
                    int32_t size = avg_minority_size;
                    if (i == 0) {
                        size = majority_size;
                    } else {
                        if (i == dop - 1) {
                            size = minority_size - (i - 1) * avg_minority_size;
                        }
                        CHECK(std::abs(avg_minority_size - size) <= dop - 1);
                    }
                    multi_nums[i].resize(size);
                    init_nums(multi_nums[i], size);
                    std::sort(multi_nums[i].begin(), multi_nums[i].end());
                });
            }
            for (int i = 0; i < dop; ++i) {
                init_threads[i].join();
            }
        } else {
            const int32_t avg_data_size = FLAGS_data_size / dop;
            std::vector<std::thread> init_threads;
            for (int i = 0; i < dop; ++i) {
                init_threads.emplace_back([&multi_nums, i, dop, avg_data_size]() {
                    int32_t size = avg_data_size;
                    if (i == dop - 1) {
                        size = FLAGS_data_size - i * avg_data_size;
                    }
                    CHECK(std::abs(avg_data_size - size) <= dop);
                    multi_nums[i].resize(size);
                    init_nums(multi_nums[i], size);
                    std::sort(multi_nums[i].begin(), multi_nums[i].end());
                });
            }
            for (int i = 0; i < dop; ++i) {
                init_threads[i].join();
            }
        }

        for (int32_t cnt = 1; cnt <= FLAGS_iteration; ++cnt) {
            T merger(multi_nums, dop, FLAGS_chunk_size, FLAGS_max_buffer_size);
            int64_t time = 0;
            {
                SCOPED_TIMER(time);
                std::vector<int32_t> merged_nums;
                while (!merger.eos()) {
                    std::vector<int32_t> chunk = merger.pull();
                    if (!chunk.empty()) {
                        merged_nums.insert(merged_nums.end(), chunk.begin(), chunk.end());
                    }
                }
                CHECK(merged_nums.size() == FLAGS_data_size);
            }
            if (time < min_time) {
                min_time = time;
            }
            if (time > max_time) {
                max_time = time;
            }
            avg_time += time;
            if (FLAGS_verbose) {
                std::cout << "\titeration: " << cnt << "/" << FLAGS_iteration
                          << ", time usage: " << time_to_string(time) << std::endl;
            } else {
                if (cnt == 1) {
                    std::cout << "\titeration: " << cnt << "/" << FLAGS_iteration << std::flush;
                } else {
                    std::cout << ", " << cnt << "/" << FLAGS_iteration << std::flush;
                }
                if (cnt == FLAGS_iteration) {
                    std::cout << std::endl;
                }
            }
        }
        avg_time = avg_time / FLAGS_iteration;
        std::cout << "\taverage time usage: " << time_to_string(avg_time) << std::endl;
        std::cout << "\tmin time usage: " << time_to_string(min_time) << std::endl;
        std::cout << "\tmax time usage: " << time_to_string(max_time) << std::endl;

        avg_times.push_back(avg_time);
        min_times.push_back(min_time);
        max_times.push_back(max_time);
    }

    auto print = [](const std::string& text, const std::string& time_unit, const std::vector<int64_t> v) {
        std::cout << text << ": ";
        for (int32_t i = 0; i < v.size(); i++) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << time_to_string(v[i], time_unit);
        }
        std::cout << std::endl;
    };

    std::cout << "overall results:" << std::endl;
    print(" -> avg_times", "s", avg_times);
    print(" -> min_times", "s", min_times);
    print(" -> max_times", "s", max_times);
}

void init_nums(std::vector<int32_t>& nums, const int32_t size) {
    if (FLAGS_data_distribution == Distribution::uniform) {
        for (int32_t i = 0; i < size; ++i) {
            nums[i] = u(e);
        }
    } else if (FLAGS_data_distribution == Distribution::normal) {
        for (int32_t i = 0; i < size; ++i) {
            nums[i] = n(e);
        }
    } else {
        CHECK(FLAGS_data_distribution == Distribution::skew);
        for (int32_t i = 0; i < size; ++i) {
            if (u_percent(e) <= FLAGS_skew_percent) {
                nums[i] = u_skew_majority(e);
            } else {
                nums[i] = u(e);
            }
        }
    }
}
