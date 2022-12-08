#pragma once

#include <stdint.h>

#include <string>
#include <vector>

class BlockingSorter {
public:
    static inline std::string name = "BlockingSorter";
    virtual void sort(std::vector<int32_t>& nums, const int32_t processor_num) = 0;
    virtual ~BlockingSorter() = default;

protected:
    std::vector<std::vector<int32_t>> _split_and_sort(const std::vector<int32_t>& nums, const int32_t processor_num);
};
