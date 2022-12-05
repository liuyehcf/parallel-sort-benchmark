#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "blocking_sorter.h"

class Block {
public:
    Block(std::vector<int32_t>& nums, const int32_t start, const int32_t end)
            : _nums(nums), _start(start), _end(end), _idx(start) {}

    std::vector<int32_t>& nums() { return _nums; }
    int32_t start() const { return _start; }
    int32_t end() const { return _end; }
    int32_t size() const { return _end - _start; }
    int32_t stat() const { return _stat; }
    bool is_neutralized() const { return _stat != NOT_NEUTRALIZED; }
    bool is_left_neutralized() const { return _stat == LEFT_NEUTRALIZED; }
    bool is_right_neutralized() const { return _stat == RIGHT_NEUTRALIZED; }

    void mark_neutralized(bool left) {
        if (left) {
            _stat = LEFT_NEUTRALIZED;
        } else {
            _stat = RIGHT_NEUTRALIZED;
        }
    }
    bool is_eos() { return _idx >= _end; }
    int32_t cur_value() { return _nums[_idx]; }
    void inc() { _idx++; }
    void swap_cur_value(Block* other) {
        int32_t tmp = _nums[_idx];
        _nums[_idx] = other->_nums[other->_idx];
        other->_nums[other->_idx] = tmp;
    }

    static constexpr int32_t LEFT_NEUTRALIZED = -1;
    static constexpr int32_t RIGHT_NEUTRALIZED = 1;
    static constexpr int32_t NOT_NEUTRALIZED = 0;

private:
    std::vector<int32_t>& _nums;

    // [start, end)
    const int32_t _start;
    const int32_t _end;
    int32_t _idx;
    int32_t _stat = NOT_NEUTRALIZED;
};

class Group;
using GroupPtr = std::shared_ptr<Group>;

class Group {
public:
    Group(std::vector<int32_t>& nums, const int32_t start, const int32_t end, const int32_t processor_num,
          std::shared_ptr<std::mutex> mutex, std::shared_ptr<std::vector<std::thread>> threads,
          std::shared_ptr<std::condition_variable> cv, std::shared_ptr<int32_t> running_tasks)
            : _nums(nums),
              _start(start),
              _end(end),
              _processor_num(processor_num),
              _mutex(mutex),
              _threads(threads),
              _cv(cv),
              _running_tasks(running_tasks) {}

    std::vector<int32_t>& nums() { return _nums; }
    std::vector<std::shared_ptr<Block>>& blocks() { return _blocks; }
    int32_t start() const { return _start; }
    int32_t end() const { return _end; }
    int32_t processor_num() const { return _processor_num; }
    int32_t pivot() const { return _pivot; }
    int32_t pivot_idx() const { return _pivot_idx; }
    void set_pivot_idx(int32_t pivot_idx) { _pivot_idx = pivot_idx; }
    int32_t& LN() { return _LN; }
    int32_t& RN() { return _RN; }

    std::shared_ptr<std::mutex> mutex() { return _mutex; }
    std::shared_ptr<std::vector<std::thread>> threads() { return _threads; }
    std::shared_ptr<std::condition_variable> cv() { return _cv; }
    std::shared_ptr<std::int32_t> running_tasks() { return _running_tasks; }

    void init(int32_t block_size) {
        // Select pivot
        _pivot = (_nums[_start] + _nums[_end - 1] + _nums[_start + ((_end - _start) >> 1)]) / 3;

        // Split to blocks
        int32_t i = _start;
        while (i + block_size <= _end) {
            _blocks.emplace_back(std::make_shared<Block>(_nums, i, i + block_size));
            i += block_size;
        }

        // Last block may smaller than the expected block size
        if (i < _end) {
            _blocks.emplace_back(std::make_shared<Block>(_nums, i, _end));
        }

        _lbi = 0;
        _rbi = _blocks.size() - 1;
    }

    Block* get_next_block(bool left) {
        std::lock_guard<std::mutex> l(_block_mutex);
        if (_lbi <= _rbi) {
            if (left) {
                return _blocks[_lbi++].get();
            } else {
                return _blocks[_rbi--].get();
            }
        } else {
            return nullptr;
        }
    }

private:
    std::vector<int32_t>& _nums;
    // [start, end)
    const int32_t _start;
    const int32_t _end;
    const int32_t _processor_num;

    int32_t _pivot;
    int32_t _pivot_idx = -1;

    std::vector<std::shared_ptr<Block>> _blocks;
    std::mutex _block_mutex;
    // left/right block index
    int32_t _lbi;
    int32_t _rbi;

    // [start, _LN] (LN, RN) [RN, end)
    int32_t _LN;
    int32_t _RN;

    // Shared by all groups
    std::shared_ptr<std::mutex> _mutex = nullptr;
    std::shared_ptr<std::vector<std::thread>> _threads = nullptr;
    std::shared_ptr<std::condition_variable> _cv = nullptr;
    std::shared_ptr<int32_t> _running_tasks = nullptr;
};

class ParallelBlockBasedQuickSorter : public BlockingSorter {
public:
    ParallelBlockBasedQuickSorter(bool check, int32_t block_size) : _check(check), _block_size(block_size) {}
    void sort(std::vector<int32_t>& nums, const int32_t processor_num) override;

private:
    void _process_group(GroupPtr group);
    void _init_group(Group* group);

    void _parallel_partition(Group* group);

    void _serial_partition(Group* group);
    void _serial_neutralize(Group* group);
    void _serial_swap(Group* group);
    void _serial_final_partition(Group* group);

    void _split_group(Group* group);

    void _neutralize(Block* left, Block* right, int32_t pivot);

    void _check_group(Group* group);

private:
    const bool _check;
    const int32_t _block_size;
};
