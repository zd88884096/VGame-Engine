cmake_minimum_required(VERSION 3.14)

project(simdUnitTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_executable(simdUnitTest "unitTest.cpp")

add_executable(simd_sse_test "simd_sse_test.cpp")
add_executable(simd_avx_test "simd_avx_test.cpp")
add_test(NAME simd_sse_test COMMAND simd_sse_test CONFIG Debug)
add_test(NAME simd_avx_test COMMAND simd_avx_test CONFIG Debug)

include_directories(../../../../include)

target_link_libraries(simd_sse_test gtestd gtest_maind gmockd gmock_maind)
