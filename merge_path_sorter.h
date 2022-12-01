#pragma once

#include <stddef.h>

#include <utility>
#include <vector>

#include "sorter.h"

class MergePathSorter : public Sorter {
public:
    void sort(std::vector<int32_t>& nums, const size_t processor_num) override;

private:
    void _merge_path_merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right,
                           std::vector<int32_t>& dest, const size_t processor_num);
    std::pair<size_t, size_t> _eval_diagnoal_intersection(const std::vector<int32_t>& left,
                                                          const std::vector<int32_t>& right, const size_t processor_idx,
                                                          const size_t processor_num);
    std::pair<bool, bool> _is_intersection(const std::vector<int32_t>& left, const size_t left_idx,
                                           const std::vector<int32_t>& right, const size_t right_idx);
    void _do_merge_along_merge_path(const std::vector<int32_t>& left, const size_t left_start,
                                    const std::vector<int32_t>& right, const size_t right_start,
                                    std::vector<int32_t>& dest, const size_t dest_start, const size_t length);
};
