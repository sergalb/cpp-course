cmake_minimum_required(VERSION 3.12)
project(fixed_vector)

include_directories(${FiIXED_VECTOR_DIR} .)
set(CMAKE_CXX_STANDARD 14)

add_executable(fixed_vector_testing   vector_tests.cpp)
add_executable(fixed_vector fixed_vector.h main.cpp)