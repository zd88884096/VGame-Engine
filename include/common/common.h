#pragma once
#include "setup.h"
#include <assert.h>
#include <cmath>


template<typename T>
void SafeRelease(T* ptr)
{
    assert(ptr != nullptr);
    delete ptr;
    ptr = nullptr;
}


constexpr size_t DYNAMIC_SIZE = 0;

