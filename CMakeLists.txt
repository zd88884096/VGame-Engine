cmake_minimum_required(VERSION 3.14)
project(vmath_library CXX)
set(CMAKE_CXX_STANDARD 20)

# Enable Testing 
enable_testing()

include_directories("include")
add_subdirectory("include")
add_subdirectory("test")
