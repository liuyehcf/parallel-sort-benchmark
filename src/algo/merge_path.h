#pragma once

#include <stddef.h>
#include <stdint.h>

#include <utility>
#include <vector>

class MergePath {
public:
    /**
     * left: left side sequence
     * l_size: size of the left side sequence
     * l_step: record how many steps forward the left side sequence goes
     * right: right side sequence
     * r_size: size of the right side sequence
     * r_step: record how many steps forward the right side sequence goes
     * dest: place to put merged sequance
     * d_size: how many values should the merge algo generate. d_size can be smaller than (l_size + r_size)
     * processor_num: degree of paralleliasm
     */
    static void merge(const int32_t* left, const size_t l_size, size_t* const l_step, const int32_t* right,
                      const size_t r_size, size_t* const r_step, int32_t* const dest, const size_t d_size,
                      const size_t processor_num);

private:
    static std::pair<size_t, size_t> _eval_diagnoal_intersection(const int32_t* left, const size_t l_size,
                                                                 const int32_t* right, const size_t r_size,
                                                                 const size_t d_size, const size_t processor_idx,
                                                                 const size_t processor_num);
    static std::pair<bool, bool> _is_intersection(const int32_t* left, const size_t l_size, const size_t li,
                                                  const int32_t* right, const size_t r_size, const size_t ri);
    static void _do_merge_along_merge_path(const int32_t* left, const size_t l_size, size_t& li, const int32_t* right,
                                           const size_t r_size, size_t& ri, int32_t* const dest, const size_t d_size,
                                           size_t& di, const size_t length);
};
