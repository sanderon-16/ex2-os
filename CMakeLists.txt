cmake_minimum_required(VERSION 3.22)
project(ex2)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex2 ThreadScheduler.cpp ThreadScheduler.h thread.cpp thread.h uthreads.h uthreads.cpp)
