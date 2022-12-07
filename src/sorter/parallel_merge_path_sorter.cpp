#include "parallel_merge_path_sorter.h"

#include "merge_path.h"

void ParallelMergePathSorter::_merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right,
                                     std::vector<int32_t>& dest, const int32_t processor_num) {
    MergePath::merge(left.data(), left.size(), nullptr, right.data(), right.size(), nullptr, dest.data(), dest.size(),
                     processor_num);
}
