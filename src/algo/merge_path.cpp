#include "merge_path.h"

#include <atomic>
#include <thread>

#include "util.h"

void MergePath::merge(const int32_t* left, const int32_t l_size, int32_t* const l_step, const int32_t* right,
                      const int32_t r_size, int32_t* const r_step, int32_t* const dest, const int32_t d_size,
                      const int32_t processor_num) {
    CHECK(l_size >= 0);
    CHECK(r_size >= 0);
    CHECK(processor_num > 0);

    if (d_size <= 0) {
        if (l_step != nullptr) {
            *l_step = 0;
        }
        if (r_step != nullptr) {
            *r_step = 0;
        }
        return;
    }

    const int32_t length = (l_size + r_size) / processor_num + 1;
    std::vector<std::thread> threads;

    std::atomic<int32_t> l_step_atomic(0);
    std::atomic<int32_t> r_step_atomic(0);

    for (int32_t i = 0; i < processor_num; ++i) {
        threads.emplace_back([left, l_size, &l_step_atomic, right, r_size, &r_step_atomic, dest, d_size, length, i,
                              processor_num]() {
            auto pair = _eval_diagnoal_intersection(left, l_size, right, r_size, d_size, i, processor_num);
            int32_t li = pair.first;
            int32_t ri = pair.second;
            int32_t di = i * (d_size) / processor_num;
            _do_merge_along_merge_path(left, l_size, li, right, r_size, ri, dest, d_size, di, length);

            int32_t l_old = l_step_atomic;
            while (li > l_old) {
                if (l_step_atomic.compare_exchange_strong(l_old, li)) {
                    break;
                }
            }
            int32_t r_old = r_step_atomic;
            while (ri > r_old) {
                if (r_step_atomic.compare_exchange_strong(r_old, ri)) {
                    break;
                }
            }
        });
    }

    for (int32_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    if (l_step != nullptr) {
        *l_step = l_step_atomic;
    }
    if (r_step != nullptr) {
        *r_step = r_step_atomic;
    }
}

std::pair<int32_t, int32_t> MergePath::_eval_diagnoal_intersection(const int32_t* left, const int32_t l_size,
                                                                   const int32_t* right, const int32_t r_size,
                                                                   const int32_t d_size, const int32_t processor_idx,
                                                                   const int32_t processor_num) {
    int32_t diag = processor_idx * d_size / processor_num;
    if (diag > d_size - 1) {
        diag = d_size - 1;
    }

    int32_t high = diag;
    int32_t low = 0;
    if (high > l_size) {
        high = l_size;
    }

    // binary search
    while (low < high) {
        int32_t i = low + (high - low) / 2;
        int32_t j = diag - i;

        auto pair = _is_intersection(left, l_size, i, right, r_size, j);
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

        auto pair = _is_intersection(left, l_size, i, right, r_size, j);
        bool is_intersection = pair.first;

        if (is_intersection) {
            return std::make_pair(i, j);
        }
    }

    CHECK(false);
    return {};
}

std::pair<bool, bool> MergePath::_is_intersection(const int32_t* left, const int32_t l_size, const int32_t li,
                                                  const int32_t* right, const int32_t r_size, const int32_t ri) {
    // M matrix is a matrix conprising of only boolean value
    // if A[i] > B[j], then M[i, j] = true
    // if A[i] <= B[j], then M[i, j] = false
    // and for the edge cases (i or j beyond the matrix), think about the merge path, with A as the vertical vector and B as the horizontal vector,
    // which goes from left top to right bottom, the positions below the merge path should be true, and otherwise should be false
    auto evaluator = [left, right, l_size, r_size](int32_t i, int32_t j) {
        if (i < 0) {
            return false;
        } else if (i >= static_cast<int32_t>(l_size)) {
            return true;
        } else if (j < 0) {
            return true;
        } else if (j >= static_cast<int32_t>(r_size)) {
            return false;
        } else {
            return left[i] > right[j];
        }
    };

    bool has_true = false;
    bool has_false = false;

    if (evaluator(static_cast<int32_t>(li) - 1, static_cast<int32_t>(ri) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(li) - 1, static_cast<int32_t>(ri))) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(li), static_cast<int32_t>(ri) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int32_t>(li), static_cast<int32_t>(ri))) {
        has_true = true;
    } else {
        has_false = true;
    }

    return std::make_pair(has_true && has_false, has_true);
}

void MergePath::_do_merge_along_merge_path(const int32_t* left, const int32_t l_size, int32_t& li, const int32_t* right,
                                           const int32_t r_size, int32_t& ri, int32_t* const dest, const int32_t d_size,
                                           int32_t& di, const int32_t length) {
    const int32_t d_start = di;
    while (di - d_start < length && di < d_size) {
        if (li >= l_size) {
            dest[di] = right[ri];
            di++;
            ri++;
        } else if (ri >= r_size) {
            dest[di] = left[li];
            di++;
            li++;
        } else if (left[li] <= right[ri]) {
            dest[di] = left[li];
            di++;
            li++;
        } else {
            dest[di] = right[ri];
            di++;
            ri++;
        }
    }
}
