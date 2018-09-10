cmake_minimum_required(VERSION 3.10)
project(Huffman)

set(CMAKE_CXX_STANDARD 11)

add_library(huffman_lib lib/library.h
                        lib/library.cpp
                        lib/Encoder.cpp
                        lib/Encoder.h
                        lib/Decoder.cpp
                        lib/Decoder.h)

add_executable(Huffman
        main.cpp
        )

add_executable(Huffman_tests
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        huffman_testing.cpp)

target_link_libraries(Huffman huffman_lib)
target_link_libraries(Huffman_tests huffman_lib)
