//
// Created by n424613 on 27/10/2022.
//
#include <cstddef>
#include <unordered_map>
//
#include <mkl.h>

void* operator new (std::size_t n) noexcept(false)
{
    int alignment = std::min(int((trunc(double(n - 1) / 8.0) + 1) * 8), 16); // max 128 bits
    return mkl_malloc(n, alignment);
}

void operator delete(void *p) noexcept
{
    mkl_free(p);
}

void* operator new[] (std::size_t n) noexcept(false)
{
    int alignment = std::min(int((trunc(double(n - 1) / 8.0) + 1) * 8), 16); // max 128 bits
    return mkl_malloc(n, alignment);
}

void operator delete[](void *p) noexcept
{
    mkl_free(p);
}
