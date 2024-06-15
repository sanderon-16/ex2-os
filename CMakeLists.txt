cmake_minimum_required(VERSION 3.22)
project(ex2)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex2 ThreadScheduler.cpp ThreadScheduler.h thread.cpp thread.h uthreads.h uthreads.cpp)
add_executable(jump demo_jmp.c)
add_executable(itimer demo_itimer.c)
add_executable(sigInt_handler demo_singInt_handler.c)