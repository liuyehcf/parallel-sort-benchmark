#pragma once

#include "parallel_merge_sorter.h"

class ParallelMergePathSorter : public ParallelMergeSorter {
public:
    static inline std::string name = "ParallelMergePathSorter";

protected:
    virtual void _merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right, std::vector<int32_t>& dest,
                        const int32_t processor_num) override;
};
