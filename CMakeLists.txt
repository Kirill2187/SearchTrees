cmake_minimum_required(VERSION 3.1)

project(SearchTrees)
set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include_directories(${SearchTrees_SOURCE_DIR}/include)
add_subdirectory(test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SearchTrees_SOURCE_DIR}/bin)




