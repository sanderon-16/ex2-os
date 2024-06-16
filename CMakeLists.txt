cmake_minimum_required(VERSION 3.22)
project(ex2)

set(CMAKE_CXX_STANDARD 11)

add_executable(ex2 thread_scheduler.cpp thread_scheduler.h thread.cpp thread.h uthreads.h uthreads.cpp tests/jona5.cpp)
