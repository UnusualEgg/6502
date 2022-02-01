cmake_minimum_required(VERSION 3.21)
project(6502)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(6502
        include/libbase.h
        src/libbase.cpp
        src/nes.cpp)
