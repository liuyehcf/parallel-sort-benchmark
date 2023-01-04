#pragma once

#include <stddef.h>
#include <stdint.h>

#include <utility>
#include <vector>

struct Segment {
    Segment(std::vector<int32_t>& data, const size_t start, const size_t len) : data(data), start(start), len(len){};

    std::vector<int32_t>& data;
    size_t start;
    size_t len;
    size_t forward;
};

class MergePath {
public:
    /**
     * left: left side segment
     * right: right side segment
     * dest: place to put merged sequance
     * processor_num: degree of paralleliasm
     */
    static void merge(Segment& left, Segment& right, Segment& dest, const size_t processor_num);

private:
    static std::pair<size_t, size_t> _eval_diagnoal_intersection(const Segment& left, const Segment& right,
                                                                 const size_t d_size, const size_t processor_idx,
                                                                 const size_t processor_num);
    static std::pair<bool, bool> _is_intersection(const Segment& left, const size_t li, const Segment& right,
                                                  const size_t ri);
    static void _do_merge_along_merge_path(const Segment& left, size_t& li, const Segment& right, size_t& ri,
                                           Segment& dest, size_t& di, const size_t length);
};
