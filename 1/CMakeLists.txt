cmake_minimum_required(VERSION 3.21)
project(1)

include_directories(/usr/include/mpi/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES Parallel.cpp)

add_executable(1 Parallel.cpp)
