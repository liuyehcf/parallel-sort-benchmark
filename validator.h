#pragma once

#include "blocking_sorter.h"

void validate();

void validate_merge_path();

template <typename T>
void validate_blocking_sorter();

template <typename T, bool test_processor_num, bool test_max_buffer_size>
void validate_stream_merger();
