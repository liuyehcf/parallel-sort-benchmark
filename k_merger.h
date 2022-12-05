#pragma once

#include "merger.h"

class KMerger : public StreamMerger {
public:
    KMerger(std::vector<std::vector<int32_t>>&& multi_nums, const int32_t chunk_size)
            : StreamMerger(std::move(multi_nums), chunk_size) {
        _offsets.assign(_multi_nums.size(), 0);
    }
    std::vector<int32_t> pull() override;
    bool eos() override { return _eos; }

private:
    void _get_next(int32_t* val);
    std::vector<int32_t> _offsets;
    bool _eos = false;
};
