#pragma once

#include "blocking_sorter.h"

class ParallelMergeSorter : public BlockingSorter {
public:
    static inline std::string name = "ParallelMergeSorter";
    virtual void sort(std::vector<int32_t>& nums, const int32_t processor_num) final override;

protected:
    virtual void _merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right, std::vector<int32_t>& dest,
                        const int32_t processor_num) = 0;
};
