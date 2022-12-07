cmake_minimum_required(VERSION 3.20)

project(VALIDATOR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-sign-compare -g")

include_directories(src/algo)
include_directories(src/common)
include_directories(src/merger)
include_directories(src/sorter)

add_subdirectory(src/algo)
add_subdirectory(src/merger)
add_subdirectory(src/sorter)

add_executable(validator ./validator.cpp)
target_link_libraries(validator algo)
target_link_libraries(validator merger)
target_link_libraries(validator sorter)
