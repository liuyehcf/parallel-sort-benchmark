#pragma once

#include "streaming_merger.h"

class SerialKMerger : public StreamMerger {
public:
    static inline std::string name = "SerialKMerger";
    SerialKMerger(const std::vector<std::vector<int32_t>>& multi_nums, const int32_t processor_num,
                  const int32_t chunk_size, const int32_t max_buffer_size)
            : StreamMerger(multi_nums, processor_num, chunk_size, max_buffer_size) {
        _offsets.assign(_multi_nums.size(), 0);
    }
    virtual std::vector<int32_t> pull() override;

private:
    void _get_next(int32_t* val);
    std::vector<int32_t> _offsets;
};
