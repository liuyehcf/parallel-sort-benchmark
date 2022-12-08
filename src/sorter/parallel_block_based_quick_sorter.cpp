#include "parallel_block_based_quick_sorter.h"

#include <algorithm>
#include <atomic>
#include <bitset>
#include <random>
#include <stdexcept>

#include "util.h"

int32_t ParallelBlockBasedQuickSorter::DEFAULT_BLOCK_SIZE = 1024;

void ParallelBlockBasedQuickSorter::sort(std::vector<int32_t>& nums, const int32_t processor_num) {
    GroupPtr group = std::make_shared<Group>(nums, 0, nums.size(), processor_num, std::make_shared<std::mutex>(),
                                             std::make_shared<std::vector<std::thread>>(),
                                             std::make_shared<std::condition_variable>(), std::make_shared<int32_t>(0));
    _process_group(group);

    while (true) {
        std::unique_lock<std::mutex> l(*group->mutex());
        if (*group->running_tasks() == 0) {
            break;
        }
        group->cv()->wait(l);
    }

    for (int i = 0; i < group->threads()->size(); ++i) {
        (*group->threads())[i].join();
    }
}

void ParallelBlockBasedQuickSorter::_process_group(GroupPtr group) {
    std::lock_guard<std::mutex> l(*group->mutex());
    (*group->running_tasks())++;
    group->threads()->emplace_back([this, group]() {
        if (group->end() > group->start()) {
            if (group->processor_num() == 1) {
                std::sort(group->nums().begin() + group->start(), group->nums().begin() + group->end());
            } else {
                _init_group(group.get());
                _parallel_partition(group.get());
                _serial_partition(group.get());
                _check_group(group.get());
                _split_group(group.get());
            }
        }
        {
            std::lock_guard<std::mutex> l(*group->mutex());
            (*group->running_tasks())--;
            if (*group->running_tasks() == 0) {
                group->cv()->notify_one();
            }
        }
    });
}

void ParallelBlockBasedQuickSorter::_init_group(Group* group) {
    CHECK(group->start() < group->end());

    group->init(_block_size);
}

void ParallelBlockBasedQuickSorter::_parallel_partition(Group* group) {
    std::vector<std::thread> threads;
    for (int32_t i = 0; i < group->processor_num(); ++i) {
        threads.emplace_back([this, &group]() {
            bool need_left = true;
            bool need_right = true;
            Block* left = nullptr;
            Block* right = nullptr;
            while (true) {
                if (need_left) {
                    left = group->get_next_block(true);
                    need_left = false;
                }
                if (need_right) {
                    right = group->get_next_block(false);
                    need_right = false;
                }
                if (left == nullptr || right == nullptr) {
                    break;
                }
                _neutralize(left, right, group->pivot());
                if (left->is_left_neutralized()) {
                    need_left = true;
                }
                if (right->is_right_neutralized()) {
                    need_right = true;
                }
            }
        });
    }
    for (int32_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
}

void ParallelBlockBasedQuickSorter::_serial_partition(Group* group) {
    _serial_neutralize(group);
    _serial_swap(group);
    _serial_final_partition(group);
}

void ParallelBlockBasedQuickSorter::_serial_neutralize(Group* group) {
    auto& blocks = group->blocks();
    auto& LN = group->LN();
    auto& RN = group->RN();
    LN = group->start() - 1;
    RN = group->end();

    for (auto& block : blocks) {
        if (block->is_left_neutralized()) {
            LN += block->size();
        } else if (block->is_right_neutralized()) {
            RN -= block->size();
        }
    }

    // left block index
    int32_t lbi = 0;
    // right block index
    int32_t rbi = blocks.size() - 1;
    while (true) {
        // Find from left to right the first unfinished block in the interval [start, LN]
        // found left unfinished block
        bool found_lub = false;
        while (lbi < rbi) {
            auto& block = blocks[lbi];
            if (!block->is_neutralized()) {
                if (block->end() - 1 <= LN) {
                    found_lub = true;
                }
                break;
            } else {
                if (block->start() > LN) {
                    break;
                }
            }
            lbi++;
        }

        // Find from right to left the first unfinished block in the interval [RN, end)
        // found left unfinished block
        bool found_rub = false;
        while (lbi < rbi) {
            auto& block = blocks[rbi];
            if (!block->is_neutralized()) {
                if (block->start() >= RN) {
                    found_rub = true;
                }
                break;
            } else {
                if (block->end() - 1 < RN) {
                    break;
                }
            }
            rbi--;
        }

        if (found_lub && found_rub) {
            // If there are two blocks located outside [LN, RN]
            // use the same process of the parallel partition stage
            Block* left = blocks[lbi].get();
            Block* right = blocks[rbi].get();
            _neutralize(left, right, group->pivot());

            if (left->is_neutralized()) {
                LN += left->size();
                lbi++;
            }

            if (right->is_neutralized()) {
                RN -= right->size();
                rbi--;
            }
            continue;
        }

        break;
    }
}

void ParallelBlockBasedQuickSorter::_serial_swap(Group* group) {
    auto& blocks = group->blocks();
    const auto& LN = group->LN();
    const auto& RN = group->RN();

    // Mark values between (LN, RN) with block stat
    std::vector<int32_t> tie;
    tie.resize((RN - 1) - (LN + 1) + 1);
    tie.assign(tie.size(), -100);
    for (int32_t bi = 0; bi < blocks.size(); bi++) {
        auto& block = blocks[bi];
        if (block->end() - 1 <= LN) {
            continue;
        }
        if (block->start() >= RN) {
            break;
        }

        const int32_t start = std::max(LN + 1, static_cast<int32_t>(block->start()));
        const int32_t end = std::min(RN - 1, static_cast<int32_t>(block->end()) - 1);
        const int32_t stat = block->stat();
        for (int32_t i = start; i <= end; ++i) {
            tie[i - (LN + 1)] = stat;
        }
    }
    for (int i = 0; i < tie.size(); ++i) {
        CHECK(tie[i] != -100);
    }

    // left block index
    int32_t lbi = 0;
    // right block index
    int32_t rbi = blocks.size() - 1;
    // left tie index
    int32_t lti = 0;
    // right tie index
    int32_t rti = tie.size() - 1;
    while (true) {
        // Find from left to right the first unfinished block that being on the left side of LN or across LN
        // found left unfinished block
        bool found_lub = false;
        while (lbi < rbi) {
            auto& block = blocks[lbi];
            if (!block->is_neutralized()) {
                // Do not need to check whether it is across LN
                if (block->start() <= LN) {
                    found_lub = true;
                }
                break;
            } else {
                if (block->start() > LN) {
                    break;
                }
            }
            lbi++;
        }
        if (found_lub) {
            auto& block = blocks[lbi];
            int32_t end = std::min(LN, static_cast<int32_t>(block->end()) - 1);
            for (int32_t i = block->start(); i <= end; i++) {
                while (lti < tie.size() && tie[lti] != Block::LEFT_NEUTRALIZED) {
                    lti++;
                }
                CHECK(lti < tie.size());
                swap(group->nums(), i, lti + (LN + 1));
                tie[lti] = Block::NOT_NEUTRALIZED;
            }
            block->mark_neutralized(true);
            lbi++;
        }

        // Find from right to left the first unfinished block that being on the right side of RN or across RN
        // found left unfinished block
        bool found_rub = false;
        while (lbi < rbi) {
            auto& block = blocks[rbi];
            if (!block->is_neutralized()) {
                // Do not need to check whether it is across RN
                if (block->end() - 1 >= RN) {
                    found_rub = true;
                }
                break;
            } else {
                if (block->end() - 1 < RN) {
                    break;
                }
            }
            block->mark_neutralized(false);
            rbi--;
        }

        if (found_rub) {
            auto& block = blocks[rbi];
            int32_t start = std::max(RN, static_cast<int32_t>(block->start()));
            for (int32_t i = start; i <= block->end() - 1; i++) {
                while (rti >= 0 && tie[rti] != Block::RIGHT_NEUTRALIZED) {
                    rti--;
                }
                CHECK(rti >= 0);
                swap(group->nums(), i, rti + (LN + 1));
                tie[rti] = Block::NOT_NEUTRALIZED;
            }
            block->mark_neutralized(false);
            rbi--;
        }
        if (!found_lub && !found_rub) {
            break;
        }
    }
}

void ParallelBlockBasedQuickSorter::_serial_final_partition(Group* group) {
    auto& LN = group->LN();
    auto& RN = group->RN();

    int32_t left = LN + 1;
    int32_t right = RN - 1;
    while (left < right) {
        while (left < right && group->nums()[left] < group->pivot()) {
            left++;
        }
        while (left < right && group->nums()[right] > group->pivot()) {
            right--;
        }
        if (left < right) {
            swap(group->nums(), left, right);
            left++;
            right--;
        }
    }

    if (group->nums()[left] <= group->pivot()) {
        group->set_pivot_idx(left);
    } else {
        group->set_pivot_idx(left - 1);
    }
}

void ParallelBlockBasedQuickSorter::_split_group(Group* group) {
    // [start, pivot_idx] and (pivot_idx, end)
    int32_t left_size = group->pivot_idx() - group->start() + 1;

    int32_t size = (group->end() - 1) - group->start() + 1;

    int32_t left_processor_num = std::max(1, left_size / size);
    int32_t right_processor_num = group->processor_num() - left_processor_num;

    GroupPtr left_group =
            std::make_shared<Group>(group->nums(), group->start(), group->pivot_idx() + 1, left_processor_num,
                                    group->mutex(), group->threads(), group->cv(), group->running_tasks());
    GroupPtr right_group =
            std::make_shared<Group>(group->nums(), group->pivot_idx() + 1, group->end(), right_processor_num,
                                    group->mutex(), group->threads(), group->cv(), group->running_tasks());

    _process_group(left_group);
    _process_group(right_group);
}

void ParallelBlockBasedQuickSorter::_neutralize(Block* left, Block* right, int32_t pivot) {
    while (!left->is_eos() && !right->is_eos()) {
        while (!left->is_eos() && left->cur_value() < pivot) {
            left->inc();
        }

        while (!right->is_eos() && right->cur_value() > pivot) {
            right->inc();
        }

        if (!left->is_eos() && !right->is_eos()) {
            left->swap_cur_value(right);
            left->inc();
            right->inc();
        }

        if (left->is_eos()) {
            left->mark_neutralized(true);
        }
        if (right->is_eos()) {
            right->mark_neutralized(false);
        }
    }
}

void ParallelBlockBasedQuickSorter::_check_group(Group* group) {
    if (!_check) {
        return;
    }
    if (group->pivot_idx() < group->start() || group->pivot_idx() >= group->end()) {
        std::cout << "block_size=" << _block_size << ", processor_num=" << group->processor_num()
                  << ", pivot_idx=" << group->pivot_idx() << ", pivot=" << group->pivot() << std::endl;
        print(group->nums(), group->start(), group->end());
        CHECK(false);
    }
    for (int32_t i = group->start(); i <= group->pivot_idx(); i++) {
        if (group->nums()[i] > group->pivot()) {
            std::cout << "block_size=" << _block_size << ", processor_num=" << group->processor_num()
                      << ", pivot_idx=" << group->pivot_idx() << ", pivot=" << group->pivot() << std::endl;
            print(group->nums(), group->start(), group->end());
            CHECK(false);
        }
    }
    for (int32_t i = group->pivot_idx() + 1; i < group->end(); i++) {
        if (group->nums()[i] < group->pivot()) {
            std::cout << "block_size=" << _block_size << ", processor_num=" << group->processor_num()
                      << ", pivot_idx=" << group->pivot_idx() << ", pivot=" << group->pivot() << std::endl;
            print(group->nums(), group->start(), group->end());
            CHECK(false);
        }
    }
}

int test_parallel_block_based_quick_sorter() {
    std::default_random_engine e(0);
    std::uniform_int_distribution<int32_t> u32(0, 100);
    for (int32_t len = 1; len < 1024; len++) {
        for (int32_t block_size = 1; block_size < 64; block_size++) {
            for (int32_t processor_num = 1; processor_num <= 16; processor_num++) {
                std::cout << "len=" << len << ", block_size=" << block_size << ", processor_num=" << processor_num
                          << std::endl;
                ParallelBlockBasedQuickSorter sorter(true, block_size);

                std::vector<int32_t> nums;
                for (int32_t i = 0; i < len; ++i) {
                    nums.push_back(u32(e));
                }

                sorter.sort(nums, processor_num);
            }
        }
    }

    return 0;
}
