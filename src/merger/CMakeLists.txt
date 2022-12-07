cmake_minimum_required(VERSION 3.20)

set(MERGER_FILES 
    ./streaming_merger.cpp
    ./serial_k_merger.cpp
    ./parallel_plain_merger.cpp
    ./parallel_merge_path_merger.cpp)

add_library(merger STATIC ${MERGER_FILES})
