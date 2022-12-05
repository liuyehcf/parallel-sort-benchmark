cmake_minimum_required(VERSION 3.20)

project(SORT_BENCHMARK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-sign-compare -g")

set(EXEC_FILES 
    ./main.cpp 
    ./parallel_plain_merge_sorter.cpp 
    ./parallel_plain_quick_sorter.cpp 
    ./parallel_block_based_quick_sorter.cpp 
    ./parallel_merge_path_sorter.cpp
    ./serial_k_merger.cpp
    ./parallel_plain_merger.cpp)

add_executable(sort_benchmark ${EXEC_FILES})

find_package(benchmark REQUIRED)

target_link_libraries(sort_benchmark benchmark::benchmark)
