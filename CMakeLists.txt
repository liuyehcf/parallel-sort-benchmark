cmake_minimum_required(VERSION 3.20)

project(SORT_BENCHMARK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -g")

set(EXEC_FILES ./main.cpp ./merge_path_sorter.cpp ./simple_merge_sorter.cpp ./simple_quick_sorter.cpp)

add_executable(sort_benchmark ${EXEC_FILES})

find_package(benchmark REQUIRED)

target_link_libraries(sort_benchmark benchmark::benchmark)
