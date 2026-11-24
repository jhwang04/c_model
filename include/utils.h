#pragma once

#include <string>
#include "ints.h"

/// Helper macro to memset an array to zero.
#define ZERO_ARRAY(arr) memset(arr, 0, sizeof(arr) * sizeof(arr[0]));

/// Helper to set a register. Upper/lower bounds are inclusive. The idea is to be similar to verilog colon notation.
/// The user is responsible for ensuring bounds are not violated.
static constexpr void SET_REGISTER(U8* reg, const U32 upper_bound, const U32 lower_bound, U64 val) {
    if (upper_bound < lower_bound) {
        for (U64 i = upper_bound; i >= lower_bound; --i) {
            reg[i] = val & 0x1;
            val >>= 1;
        }
        return;
    }

    for (U64 i = lower_bound; i <= upper_bound; ++i) {
        reg[i] = val & 0x1;
        val >>= 1;
    }
}

/// Helper to report an error.
static constexpr void REPORT_ERROR(const std::string& err) {
    printf("ERROR: ");
    printf("%s\n", err.c_str());
}