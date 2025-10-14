cmake_minimum_required(VERSION 4.0)
project(cma C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(cma main.c
        alloc.c
        alloc.h
        block_algorithm.c
        block_algorithm.h)
