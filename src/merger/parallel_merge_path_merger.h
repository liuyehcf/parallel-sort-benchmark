#pragma once

#include <memory>
#include <thread>

#include "streaming_merger.h"

class ParallelMergePathInternalNode : public InternalNode {
public:
    ParallelMergePathInternalNode(const int32_t chunk_size, const int32_t max_buffer_size, const NodePtr& left,
                                  const NodePtr& right, const int32_t processor_num)
            : InternalNode(chunk_size, max_buffer_size, left, right), _processor_num(processor_num) {}

protected:
    virtual void _process(std::vector<int32_t>& left, int32_t& li, bool& l_need_more, std::vector<int32_t>& right,
                          int32_t& ri, bool& r_need_more) override;

private:
    const int32_t _processor_num;
};

class ParallelMergePathMerger : public StreamMerger {
public:
    static inline std::string name = "ParallelMergePathMerger";
    ParallelMergePathMerger(const std::vector<std::vector<int32_t>>& multi_nums, const int32_t processor_num,
                            const int32_t chunk_size, const int32_t max_buffer_size)
            : StreamMerger(multi_nums, processor_num, chunk_size, max_buffer_size) {
        _init();
    }
    virtual std::vector<int32_t> pull() override;

private:
    void _init();
    void _start(const NodePtr& node);
};
