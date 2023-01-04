#pragma once

#include "parallel_merge_sorter.h"

class ParallelMergePathSorter : public ParallelMergeSorter {
public:
    static inline std::string name = "ParallelMergePathSorter";

protected:
    virtual void _merge(std::vector<int32_t>& left, std::vector<int32_t>& right, std::vector<int32_t>& dest,
                        const int32_t processor_num) override;
};
