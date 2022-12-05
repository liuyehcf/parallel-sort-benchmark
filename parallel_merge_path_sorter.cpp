#include "parallel_merge_path_sorter.h"

#include <algorithm>
#include <exception>
#include <mutex>
#include <thread>

#include "util.h"

void ParallelMergePathSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    std::vector<std::vector<int32_t>> partial_nums;
    partial_nums.resize(processor_num);

    for (int32_t i = 0; i < processor_num; ++i) {
        partial_nums[i].reserve(nums.size() / processor_num);
    }

    for (int32_t i = 0; i < nums.size(); ++i) {
        partial_nums[i % processor_num].push_back(nums[i]);
    }

    for (int32_t i = 0; i < processor_num; ++i) {
        std::sort(partial_nums[i].begin(), partial_nums[i].end());
    }

    std::mutex m;
    std::vector<std::vector<int32_t>> current_level = std::move(partial_nums);

    while (current_level.size() > 1) {
        std::vector<std::vector<int32_t>> next_level;

        const auto current_size = current_level.size();
        const auto avg_processor_num = processor_num / (current_size / 2);
        CHECK(avg_processor_num >= 1);

        int32_t i = 0;
        std::vector<std::thread> threads;
        for (; i + 1 < current_size; i += 2) {
            threads.emplace_back([this, &m, &current_level, &next_level, avg_processor_num, i]() {
                auto& left = current_level[i];
                auto& right = current_level[i + 1];
                std::vector<int32_t> merged;
                merged.resize(left.size() + right.size());

                _merge_path_merge(left, right, merged, avg_processor_num);

                std::lock_guard<std::mutex> l(m);
                next_level.emplace_back(std::move(merged));
            });
        }
        for (int32_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }

        if (i < current_size) {
            next_level.emplace_back(std::move(current_level[i]));
        }

        current_level = std::move(next_level);
    }

    std::swap(nums, current_level[0]);
}

void ParallelMergePathSorter::_merge_path_merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right,
                                                std::vector<int32_t>& dest, const int32_t processor_num) {
    int32_t length = (left.size() + right.size()) / processor_num + 1;
    std::vector<std::thread> threads;
    for (int32_t i = 0; i <= processor_num; ++i) {
        threads.emplace_back([this, &left, &right, &dest, length, i, processor_num]() {
            auto pair = _eval_diagnoal_intersection(left, right, i, processor_num);
            auto left_start = pair.first;
            auto right_start = pair.second;
            auto dest_start = i * (left.size() + right.size()) / processor_num;
            _do_merge_along_merge_path(left, left_start, right, right_start, dest, dest_start, length);
        });
    }

    for (int32_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
}

std::pair<int32_t, int32_t> ParallelMergePathSorter::_eval_diagnoal_intersection(const std::vector<int32_t>& left,
                                                                                 const std::vector<int32_t>& right,
                                                                                 const int32_t processor_idx,
                                                                                 const int32_t processor_num) {
    int32_t diag = processor_idx * (left.size() + right.size()) / processor_num;
    if (diag > left.size() + right.size() - 1) {
        diag = left.size() + right.size() - 1;
    }

    int32_t high = diag;
    int32_t low = 0;
    if (high > left.size()) {
        high = left.size();
    }

    // binary search
    while (low < high) {
        int32_t i = low + (high - low) / 2;
        int32_t j = diag - i;

        auto pair = _is_intersection(left, i, right, j);
        bool is_intersection = pair.first;
        bool all_true = pair.second;

        if (is_intersection) {
            return std::make_pair(i, j);
        } else if (all_true) {
            high = i;
        } else {
            low = i + 1;
        }
    }

    // edge cases
    for (int32_t offset = 0; offset <= 1; offset++) {
        int32_t i = low + offset;
        int32_t j = diag - i;

        auto pair = _is_intersection(left, i, right, j);
        bool is_intersection = pair.first;

        if (is_intersection) {
            return std::make_pair(i, j);
        }
    }

    CHECK(false);
}

std::pair<bool, bool> ParallelMergePathSorter::_is_intersection(const std::vector<int32_t>& left,
                                                                const int32_t left_idx,
                                                                const std::vector<int32_t>& right,
                                                                const int32_t right_idx) {
    // M matrix is a matrix conprising of only boolean value
    // if A[i] > B[j], then M[i, j] = true
    // if A[i] <= B[j], then M[i, j] = false
    // and for the edge cases (i or j beyond the matrix), think about the merge path, with A as the vertical vector and B as the horizontal vector,
    // which goes from left top to right bottom, the positions below the merge path should be true, and otherwise should be false
    auto evaluator = [&left, &right](int32_t i, int32_t j) {
        if (i < 0) {
            return false;
        } else if (i >= static_cast<int32_t>(left.size())) {
            return true;
        } else if (j < 0) {
            return true;
        } else if (j >= static_cast<int32_t>(right.size())) {
            return false;
        } else {
            return left[i] > right[j];
        }
    };

    bool has_true = false;
    bool has_false = false;

    if (evaluator(static_cast<int32_t>(left_idx) - 1, static_cast<int32_t>(right_idx) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(left_idx) - 1, static_cast<int32_t>(right_idx))) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(left_idx), static_cast<int32_t>(right_idx) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(left_idx), static_cast<int32_t>(right_idx))) {
        has_true = true;
    } else {
        has_false = true;
    }

    return std::make_pair(has_true && has_false, has_true);
}

void ParallelMergePathSorter::_do_merge_along_merge_path(const std::vector<int32_t>& left, const int32_t left_start,
                                                         const std::vector<int32_t>& right, const int32_t right_start,
                                                         std::vector<int32_t>& dest, const int32_t dest_start,
                                                         const int32_t length) {
    int32_t i = left_start;
    int32_t j = right_start;
    int32_t k = dest_start;

    while (k - dest_start < length && k < dest.size()) {
        if (i >= left.size()) {
            dest[k] = right[j];
            k++;
            j++;
        } else if (j >= right.size()) {
            dest[k] = left[i];
            k++;
            i++;
        } else if (left[i] <= right[j]) {
            dest[k] = left[i];
            k++;
            i++;
        } else {
            dest[k] = right[j];
            k++;
            j++;
        }
    }
}
