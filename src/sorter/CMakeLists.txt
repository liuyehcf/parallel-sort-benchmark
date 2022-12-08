cmake_minimum_required(VERSION 3.20)

set(SORTER_FILES 
    ./blocking_sorter.cpp
    ./parallel_plain_quick_sorter.cpp 
    ./parallel_block_based_quick_sorter.cpp 
    ./serial_merge_sorter.cpp
    ./parallel_merge_sorter.cpp
    ./parallel_plain_merge_sorter.cpp 
    ./parallel_merge_path_sorter.cpp)

add_library(sorter STATIC ${SORTER_FILES})
