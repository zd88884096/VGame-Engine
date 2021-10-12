#pragma once
#ifndef VMATH_SETUP
#include <cassert>
#define VMATH_SETUP


#define VMATH_VERSION_MAJOR 0
#define VMATH_VERSION_MINOR 1
#define VMATH_VERSION_PATCH 0
#define VMATH_VERSION_REVSION 0
#define VMATH_VERSION 100


#define VMATH_ENABLED 1
#define VMATH_DISABLED 0

/*
 * More to do........
 */

#define FUTURE_CHANGE_MARK
#define VMATH_API 
#define VMATH_CONSTEXPR constexpr
#define VMATH_INLINE inline


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include<concepts>
#endif
#include<cstdint>
#include <intrin.h>


#define __SSE4_1__
#define __SSE4_2__
#define __AVX__
#define __AVX2__
#if defined(__SSE4_1__) && defined(__SSE4_2__)
    #define VMATH_SSE
    constexpr bool VMATH_ENABLE_SSE = true;
#else
    constexpr bool VMATH_ENABLE_SSE = false;
#endif

#if defined(__AVX__) && defined(__AVX2__)
    #define VMATH_AVX
    constexpr bool VMATH_ENABLE_AVX = true;
#else
    constexpr bool VMATH_ENABLE_AVX = false;
#endif

constexpr bool VMATH_ENABLE_SIMD = VMATH_ENABLE_AVX || VMATH_ENABLE_SSE;

#endif
