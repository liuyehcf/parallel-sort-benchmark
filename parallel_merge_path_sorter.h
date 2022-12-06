#pragma once

#include "blocking_sorter.h"

class ParallelMergePathSorter : public BlockingSorter {
public:
    virtual void sort(std::vector<int32_t>& nums, const int32_t processor_num) override;
};
