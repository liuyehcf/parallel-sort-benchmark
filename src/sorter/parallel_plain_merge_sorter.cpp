#include "parallel_plain_merge_sorter.h"

#include <algorithm>

void ParallelPlainMergeSorter::_merge(std::vector<int32_t>& left, std::vector<int32_t>& right,
                                      std::vector<int32_t>& dest, const int32_t processor_num) {
    std::merge(left.begin(), left.end(), right.begin(), right.end(), dest.begin());
}
