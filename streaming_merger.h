#pragma once

#include <stdint.h>

#include <vector>

class StreamMerger {
public:
    StreamMerger(std::vector<std::vector<int32_t>>&& multi_nums, const int32_t chunk_size)
            : _multi_nums(std::move(multi_nums)), _chunk_size(chunk_size) {}
    virtual std::vector<int32_t> pull() = 0;
    bool eos() { return _eos; }

protected:
    const std::vector<std::vector<int32_t>> _multi_nums;
    const int32_t _chunk_size;
    bool _eos = false;
};
