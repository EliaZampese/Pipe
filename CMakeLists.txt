cmake_minimum_required(VERSION 4.2)
project(Pipe C)

set(CMAKE_C_STANDARD 11)

add_executable(Pipe main.c
        cmake-build-debug/1.c
        2.c
        3.c)
