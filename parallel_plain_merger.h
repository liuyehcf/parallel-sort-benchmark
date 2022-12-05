#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "streaming_merger.h"

class Node;
using NodePtr = std::shared_ptr<Node>;

class Node {
    friend class InternalNode;

public:
    Node(const int32_t chunk_size, const int32_t max_buffer_size)
            : _chunk_size(chunk_size), _max_buffer_size(max_buffer_size) {}
    virtual ~Node() = default;
    virtual void process() = 0;

    bool is_buffer_empty() { return _buffers.empty(); }
    void fetch_from_buffer(std::vector<int32_t>& output);

    bool eos() { return _eos; }

protected:
    void _buffer_enqueue(std::unique_lock<std::mutex>& l, bool force);

    std::mutex _m;
    std::condition_variable _empty_cv;
    std::condition_variable _full_cv;
    const int32_t _chunk_size;
    const int32_t _max_buffer_size;
    std::queue<std::vector<int32_t>> _buffers;
    std::vector<int32_t> _buffer;
    std::atomic_bool _eos = {false};
};

class InternalNode : public Node {
public:
    InternalNode(const int32_t chunk_size, const int32_t max_buffer_size, const NodePtr& left, const NodePtr& right)
            : Node(chunk_size, max_buffer_size), _left(left), _right(right) {}
    void process() override;

private:
    NodePtr _left;
    NodePtr _right;
};

class LeafNode : public Node {
public:
    LeafNode(const int32_t chunk_size, const int32_t max_buffer_size, const std::vector<int32_t>& nums)
            : Node(chunk_size, max_buffer_size), _nums(nums) {}
    void process() override;

private:
    const std::vector<int32_t>& _nums;
};

class ParallelPlainMerger : public StreamMerger {
public:
    ParallelPlainMerger(std::vector<std::vector<int32_t>>&& multi_nums, const int32_t chunk_size,
                        const int32_t max_buffer_size = 64)
            : StreamMerger(std::move(multi_nums), chunk_size), _max_buffer_size(max_buffer_size) {
        _init();
    }
    ~ParallelPlainMerger();
    std::vector<int32_t> pull() override;

private:
    void _init();
    void _start(const NodePtr& node);

    const int32_t _max_buffer_size;
    std::vector<std::thread> _threads;
    NodePtr _root;
};
