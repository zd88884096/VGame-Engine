cmake_minimum_required(VERSION 3.14)

project(vectorUnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(vectorUnitTest compileTest.cpp)

#[[set(CMAKE_TOOLCHAIN_FILE "C:/Users/Administrator/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(glm CONFIG REQUIRED)
target_link_libraries(vectorUnitTest PRIVATE glm)]]
