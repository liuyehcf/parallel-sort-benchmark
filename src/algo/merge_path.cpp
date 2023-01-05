#include "merge_path.h"

#include <atomic>
#include <thread>

#include "util.h"

void MergePath::merge(Segment& left, Segment& right, Segment& dest, const size_t processor_num) {
    CHECK(processor_num > 0);

    if (dest.len == 0) {
        left.forward = 0;
        right.forward = 0;
        return;
    }

    std::vector<std::thread> threads;
    std::atomic<size_t> forward_length(0);

    for (size_t processor_idx = 0; processor_idx < processor_num; ++processor_idx) {
        threads.emplace_back([&left, &right, &dest, &forward_length, processor_idx, processor_num]() {
            size_t li;
            size_t ri;
            _eval_diagnoal_intersection(left, right, dest.len, processor_idx, processor_num, li, ri);
            size_t di = dest.start + (processor_idx * dest.len / processor_num);

            size_t next_di = dest.start + ((processor_idx + 1) * dest.len / processor_num);
            const size_t length = next_di - di;
            forward_length += length;

            _do_merge_along_merge_path(left, li, right, ri, dest, di, length);

            if (processor_idx == processor_num - 1) {
                left.forward = li - left.start;
                right.forward = ri - right.start;
            }
        });
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
    CHECK(forward_length == dest.len);
}

void MergePath::_eval_diagnoal_intersection(const Segment& left, const Segment& right, const size_t d_size,
                                            const size_t processor_idx, const size_t processor_num, size_t& li,
                                            size_t& ri) {
    const size_t diag = processor_idx * d_size / processor_num;
    CHECK(diag < d_size);

    size_t high = diag;
    size_t low = 0;
    if (high > left.len) {
        high = left.len;
    }

    bool has_true;
    bool has_false;

    // binary search
    while (low < high) {
        size_t l_offset = low + (high - low) / 2;
        CHECK(l_offset <= diag);
        size_t r_offset = diag - l_offset;

        _is_intersection(left, left.start + l_offset, right, right.start + r_offset, has_true, has_false);
        bool is_intersection = has_true && has_false;

        if (is_intersection) {
            li = left.start + l_offset;
            ri = right.start + r_offset;
            return;
        } else if (has_true) {
            high = l_offset;
        } else {
            low = l_offset + 1;
        }
    }

    // edge cases
    for (size_t offset = 0; offset <= 1; offset++) {
        size_t l_offset = low + offset;
        if (l_offset > diag) {
            break;
        }
        size_t r_offset = diag - l_offset;

        _is_intersection(left, left.start + l_offset, right, right.start + r_offset, has_true, has_false);
        bool is_intersection = has_true && has_false;

        if (is_intersection) {
            li = left.start + l_offset;
            ri = right.start + r_offset;
            return;
        }
    }

    CHECK(false);
}

void MergePath::_is_intersection(const Segment& left, const size_t li, const Segment& right, const size_t ri,
                                 bool& has_true, bool& has_false) {
    // M matrix is a matrix conprising of only boolean value
    // if A[i] > B[j], then M[i, j] = true
    // if A[i] <= B[j], then M[i, j] = false
    // and for the edge cases (i or j beyond the matrix), think about the merge path, with A as the vertical vector and B as the horizontal vector,
    // which goes from left top to right bottom, the positions below the merge path should be true, and otherwise should be false
    auto evaluator = [&left, &right](int64_t lii, int64_t rii) {
        if (lii < static_cast<int64_t>(left.start)) {
            return false;
        } else if (lii >= static_cast<int64_t>(left.start + left.len)) {
            return true;
        } else if (rii < static_cast<int64_t>(right.start)) {
            return true;
        } else if (rii >= static_cast<int64_t>(right.start + right.len)) {
            return false;
        } else {
            return left.data[lii] > right.data[rii];
        }
    };

    has_true = false;
    has_false = false;

    if (evaluator(static_cast<int64_t>(li) - 1, static_cast<int64_t>(ri) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int64_t>(li) - 1, static_cast<int64_t>(ri))) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int64_t>(li), static_cast<int64_t>(ri) - 1)) {
        has_true = true;
    } else {
        has_false = true;
    }
    if (evaluator(static_cast<int64_t>(li), static_cast<int64_t>(ri))) {
        has_true = true;
    } else {
        has_false = true;
    }
}

void MergePath::_do_merge_along_merge_path(const Segment& left, size_t& li, const Segment& right, size_t& ri,
                                           Segment& dest, size_t& di, const size_t length) {
    const size_t d_start = di;
    while (di - d_start < length && di < dest.start + dest.len) {
        if (li >= left.start + left.len) {
            dest.data[di] = right.data[ri];
            di++;
            ri++;
        } else if (ri >= right.start + right.len) {
            dest.data[di] = left.data[li];
            di++;
            li++;
        } else if (left.data[li] <= right.data[ri]) {
            dest.data[di] = left.data[li];
            di++;
            li++;
        } else {
            dest.data[di] = right.data[ri];
            di++;
            ri++;
        }
    }
}
