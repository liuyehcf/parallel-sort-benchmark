#pragma once

#include <stdint.h>

#include <utility>
#include <vector>

class MergePath {
public:
    static void merge(const int32_t* left, const int32_t l_size, int32_t* const l_step, const int32_t* right,
                      const int32_t r_size, int32_t* const r_step, int32_t* const dest, const int32_t d_size,
                      const int32_t processor_num);

private:
    static std::pair<int32_t, int32_t> _eval_diagnoal_intersection(const int32_t* left, const int32_t l_size,
                                                                   const int32_t* right, const int32_t r_size,
                                                                   const int32_t d_size, const int32_t processor_idx,
                                                                   const int32_t processor_num);
    static std::pair<bool, bool> _is_intersection(const int32_t* left, const int32_t l_size, const int32_t li,
                                                  const int32_t* right, const int32_t r_size, const int32_t ri);
    static void _do_merge_along_merge_path(const int32_t* left, const int32_t l_size, int32_t& li, const int32_t* right,
                                           const int32_t r_size, int32_t& ri, int32_t* const dest, const int32_t d_size,
                                           int32_t& di, const int32_t length);
};
