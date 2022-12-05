#pragma once

#include <stdint.h>

#include <vector>

class Sorter {
public:
    virtual void sort(std::vector<int32_t>& nums, const int32_t processor_num) = 0;
    virtual ~Sorter() = default;
};
