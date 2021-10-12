cmake_minimum_required(VERSION 3.14)

project(vector_operator_UnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O3 -march=native -DNDEBUG")
add_executable(vector_operator_UnitTest unitTest.cpp)

