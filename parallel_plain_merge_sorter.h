#pragma once

#include "blocking_sorter.h"

class ParallelPlainMergeSorter : public BlockingSorter {
public:
    void sort(std::vector<int32_t>& nums, const int32_t processor_numums);
};
