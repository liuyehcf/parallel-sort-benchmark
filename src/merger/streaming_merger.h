#pragma once

#include <stdint.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class Node;
using NodePtr = std::shared_ptr<Node>;

class Node {
    friend class InternalNode;

public:
    Node(const int32_t chunk_size, const int32_t max_buffer_size)
            : _chunk_size(chunk_size), _max_buffer_size(max_buffer_size) {}
    virtual ~Node() = default;
    virtual void process() = 0;

    bool has_more_intput() { return _has_more_intput; }
    bool is_buffer_empty() { return _is_buffer_empty; }
    bool eos() { return !has_more_intput() && is_buffer_empty(); }
    void fetch_from_buffer(std::vector<int32_t>& output);

    void enqueue_buffer(std::unique_lock<std::mutex>& l, bool force);

protected:
    const int32_t _chunk_size;
    const int32_t _max_buffer_size;
    std::mutex _m;
    std::condition_variable _empty_cv;
    std::condition_variable _full_cv;
    std::queue<std::vector<int32_t>> _buffers;
    std::vector<int32_t> _buffer;
    std::atomic_bool _has_more_intput = {true};
    std::atomic_bool _is_buffer_empty = {true};
};

class InternalNode : public Node {
public:
    InternalNode(const int32_t chunk_size, const int32_t max_buffer_size, const NodePtr& left, const NodePtr& right)
            : Node(chunk_size, max_buffer_size), _l_child(left), _r_child(right) {}
    virtual void process() final override;

protected:
    virtual void _process(const std::vector<int32_t>& left, int32_t& li, bool& l_need_more,
                          const std::vector<int32_t>& right, int32_t& ri, bool& r_need_more) = 0;
    NodePtr _l_child;
    NodePtr _r_child;
};

class LeafNode final : public Node {
public:
    LeafNode(const int32_t chunk_size, const int32_t max_buffer_size, const std::vector<int32_t>& nums)
            : Node(chunk_size, max_buffer_size), _nums(nums) {}
    virtual void process() override;

private:
    const std::vector<int32_t>& _nums;
};

class StreamMerger {
public:
    static inline std::string name = "StreamMerger";
    StreamMerger(const std::vector<std::vector<int32_t>>& multi_nums, const int32_t processor_num,
                 const int32_t chunk_size, const int32_t max_buffer_size)
            : _multi_nums(multi_nums),
              _processor_num(processor_num),
              _chunk_size(chunk_size),
              _max_buffer_size(max_buffer_size) {}
    virtual ~StreamMerger();
    virtual std::vector<int32_t> pull() = 0;
    bool eos() { return _eos; }

protected:
    const std::vector<std::vector<int32_t>> _multi_nums;
    const int32_t _processor_num;
    const int32_t _chunk_size;
    const int32_t _max_buffer_size;
    std::vector<std::thread> _threads;
    NodePtr _root;
    bool _eos = false;
};
