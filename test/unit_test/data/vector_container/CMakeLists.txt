cmake_minimum_required(VERSION 3.14)

project(vector_container_UnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -DNDEBUG -O3 -march=native")

add_executable(vector_container_UnitTest "unitTest.cpp")
#[[set(CMAKE_TOOLCHAIN_FILE "C:/Users/Administrator/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(glm CONFIG REQUIRED)
target_link_libraries(vector_container_UnitTest PRIVATE glm)
find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(vector_container_UnitTest PRIVATE Eigen3::Eigen)]]
