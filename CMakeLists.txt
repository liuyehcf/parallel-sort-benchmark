cmake_minimum_required(VERSION 3.20)

project(BENCHMARK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-sign-compare -g")

include_directories(src/algo)
include_directories(src/common)
include_directories(src/merger)
include_directories(src/sorter)

add_subdirectory(src/algo)
add_subdirectory(src/merger)
add_subdirectory(src/sorter)

add_executable(benchmark ./benchmark.cpp ./validator.cpp)
target_link_libraries(benchmark algo)
target_link_libraries(benchmark merger)
target_link_libraries(benchmark sorter)

find_package(gflags REQUIRED)
target_link_libraries(benchmark ${GFLAGS_LIBRARIES})

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}
    )

