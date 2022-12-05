#include "k_merger.h"

#include <limits>

#include "util.h"

std::vector<int32_t> KMerger::pull() {
    std::vector<int32_t> res;
    if (eos()) {
        return res;
    }
    int32_t cnt = 0;
    res.reserve(_chunk_size);
    while (cnt < _chunk_size) {
        int32_t next;
        _get_next(&next);
        if (eos()) {
            break;
        }
        res.push_back(next);
        cnt++;
    }
    return res;
}

void KMerger::_get_next(int32_t* val) {
    if (eos()) {
        return;
    }
    int64_t min = std::numeric_limits<int64_t>::max();
    int32_t min_idx = -1;
    for (int32_t i = 0; i < _multi_nums.size(); i++) {
        const auto& nums = _multi_nums[i];
        int32_t offset = _offsets[i];

        if (offset < nums.size() && nums[offset] < min) {
            min_idx = i;
            min = nums[offset];
        }
    }
    if (min_idx == -1) {
        _eos = true;
    } else {
        _offsets[min_idx]++;
        *val = min;
    }
}

int test_k_merger() {
    std::vector<int32_t> v1{1, 2, 3, 4, 5};
    std::vector<int32_t> v2{1, 2, 3, 4, 5};
    std::vector<int32_t> v3{1, 2, 3, 4, 5};

    std::vector<std::vector<int32_t>> multi_nums{v1, v2, v3};
    KMerger merger(std::move(multi_nums), 1);

    while (!merger.eos()) {
        auto&& nums = merger.pull();
        print(nums);
    }
}
