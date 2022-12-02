#pragma once
#include "sorter.h"

class SimpleQuickSorter : public Sorter {
public:
    void sort(std::vector<int32_t>& nums, const int32_t processor_num);

private:
    int32_t _partition(std::vector<int32_t>& nums, int32_t start, int32_t end);
    void _swap(std::vector<int32_t>& nums, int32_t i, int32_t j);
};
