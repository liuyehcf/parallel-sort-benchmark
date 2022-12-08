#pragma once

#include "blocking_sorter.h"

class SerialMergeSorter : public BlockingSorter {
public:
    static inline std::string name = "SerialMergeSorter";
    virtual void sort(std::vector<int32_t>& nums, const int32_t processor_num);
};
