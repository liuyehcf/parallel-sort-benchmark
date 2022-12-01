#pragma once

#include <stddef.h>

#include "sorter.h"

class SimpleMergeSorter : public Sorter {
public:
    void sort(std::vector<int32_t>& nums, const size_t processor_numums);

private:
    void _merge(const std::vector<int32_t>& left, const std::vector<int32_t>& right, std::vector<int32_t>& dest);
};
