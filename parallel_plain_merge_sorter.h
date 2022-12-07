#pragma once

#include "parallel_merge_sorter.h"

class ParallelPlainMergeSorter : public ParallelMergeSorter {
protected:
    virtual void _merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right, std::vector<int32_t>& dest,
                        const int32_t processor_num) override;
};
