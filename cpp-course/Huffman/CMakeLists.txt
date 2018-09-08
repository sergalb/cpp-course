cmake_minimum_required(VERSION 3.10)
project(Huffman)

set(CMAKE_CXX_STANDARD 11)

add_executable(Huffman
        lib/library.h
        lib/library.cpp
        #main.cpp
        lib/Encoder.cpp
        lib/Encoder.h
        lib/Decoder.cpp
        lib/Decoder.h
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        huffman_testing.cpp)
