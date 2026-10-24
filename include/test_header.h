#pragma once

#include <cstdint>
#include "ints.h"

/// Helper to swap two integers.
void my_swap(U32& a, U32& b)
{
    U32 tmp = a;
    a = b;
    b = tmp;
}
