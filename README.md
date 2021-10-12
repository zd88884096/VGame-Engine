# VGame-Engine



-   A math library with **.h files only** for compile time computation, utilized template metaprogramming and system level acceleration and supports cross-platform compilation with cmake.
-   Comes with a **custom Memory Allocator** based on Integral-type-supporting Typelist and optimized / modified red-black tree, seg-list, Memory Arena, and small object allocation. Achieved **Amortized O(1)** time complexity for freeing memory with sliding window and implicit free-list. Compared speed with mainstream math library, with maximum speed **51 times faster than malloc.** Supports **SIMD acceleration on SSE and AVX.** Currently working on thread safety, matrix operations.


# Folders

## Include

	Main code where all the contents of the library is located.

**Common**
Common math operations such as add, sub, trunc.

**Core**
Simple vector operations.

**data**
Common vector operations.

**memory**
Memory allocation files.

**simd**
Simd acceleration files.

## Test

	Contains test files for each file in Include.

## Third party / googletest

	Googletest testing framework.