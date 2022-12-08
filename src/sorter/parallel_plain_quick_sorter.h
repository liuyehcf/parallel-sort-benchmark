#pragma once
#include "blocking_sorter.h"

class ParallelPlainQuickSorter : public BlockingSorter {
public:
    static inline std::string name = "ParallelPlainQuickSorter";
    void sort(std::vector<int32_t>& nums, const int32_t processor_num);

private:
    int32_t _partition(std::vector<int32_t>& nums, int32_t start, int32_t end);
};
