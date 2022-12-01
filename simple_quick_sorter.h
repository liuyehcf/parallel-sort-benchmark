#pragma once
#include "sorter.h"

class SimpleQuickSorter : public Sorter {
public:
    void sort(std::vector<int32_t>& nums, const size_t processor_num);

private:
    size_t _partition(std::vector<int32_t>& nums, size_t start, size_t end);
    void _swap(std::vector<int32_t>& nums, size_t i, size_t j);
};
