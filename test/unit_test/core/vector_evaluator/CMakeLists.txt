cmake_minimum_required(VERSION 3.14)

project(vector_evaluator_UnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -DNDEBUG -O3 -march=native")
add_executable(vector_evaluator_UnitTest unitTest.cpp)
if(MSVC)
	target_compile_options(vector_evaluator_UnitTest PRIVATE /arch:AVX2)
endif()
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Administrator/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake")
