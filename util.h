#pragma once

#include <stdint.h>

#include <iostream>
#include <vector>

template <typename T>
inline void print(const std::vector<T>& nums, int32_t start, int32_t end) {
    for (int32_t i = start; i < end; i++) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << nums[i];
    }
    std::cout << std::endl;
}

template <typename T>
inline void print(const std::vector<T>& nums) {
    print(nums, 0, nums.size());
}

inline void swap(std::vector<int32_t>& nums, int32_t i, int32_t j) {
    if (i == j) {
        return;
    }
    int32_t tmp = nums[i];
    nums[i] = nums[j];
    nums[j] = tmp;
}