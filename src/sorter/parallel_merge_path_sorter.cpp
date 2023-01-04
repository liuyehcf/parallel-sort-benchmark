#include "parallel_merge_path_sorter.h"

#include "merge_path.h"

void ParallelMergePathSorter::_merge(std::vector<int32_t>& left, std::vector<int32_t>& right,
                                     std::vector<int32_t>& dest, const int32_t processor_num) {
    Segment s_left(left, 0, left.size());
    Segment s_right(right, 0, right.size());
    Segment s_dest(dest, 0, dest.size());
    MergePath::merge(s_left, s_right, s_dest, processor_num);
}
