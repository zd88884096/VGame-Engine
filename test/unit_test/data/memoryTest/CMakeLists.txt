cmake_minimum_required(VERSION 3.14)

project(memoryUnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -march=native")
add_executable(compileTest compileTest.cpp)