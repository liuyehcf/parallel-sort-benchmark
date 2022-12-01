#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

class Sorter {
public:
    virtual void sort(std::vector<int32_t>& nums, const size_t processor_num) = 0;
};
