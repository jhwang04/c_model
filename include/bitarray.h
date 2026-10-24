#pragma once

#include <ints.h>
#include <stdio.h>
#include <string>

/// Helper class to store an array of bits compactly.
template <U64 n> class BitArray {
 public:  
    /// Guaranteed to be large enough. May have padding.
    static constexpr U64 backing_size = (n / 64) + 1;

    /// Public-facing constructor. Zero-inits backing array.
    BitArray() {
        for (U64 i = 0; i < backing_size; ++i) {
            m_arr[i] = 0;
        }
    }      

    /// Helper function to clear all bits.
    constexpr void ClearAll() {
        for (U64 i = 0; i < backing_size; ++i) {
            m_arr[i] = 0;
        }
    }

    /// Helper function to check whether a bit is set or not.
    constexpr bool IsBitSet(U64 i) const {
        const U64 backing_i = i / U64(64);
        const U64 remainder = i % U64(64);
        const U64 mask = U64(1) << remainder;
        return (m_arr[backing_i] & mask);
    }

    /// Helper function to set a bit.
    constexpr void SetBit(U64 i) {
        const U64 backing_i = i / 64;
        const U64 remainder = i % 64;
        const U64 mask = U64(1) << remainder;
        m_arr[backing_i] |= mask;
    }

    /// Shifts all elements up one index. Input is the new bit to shift.
    constexpr void ShiftUp(bool in) {
        for (U64 i = backing_size - 1; i > 0; --i) {
            m_arr[i] <<= 1;
            m_arr[i] &= (m_arr[i - 1] & top_bit) ? 1 : 0;
        }
        m_arr[0] <<= 1;
        m_arr[0] &= in ? 1 : 0;
    }

    /// Shifts all elements down one index. Input is the new bit.
    constexpr void ShiftDown(bool in) {
        m_arr[backing_size - 1] |= in ? 1 << (n % 64) : 0;      
        for (U64 i = 0; i < backing_size - 1; ++i) {
            m_arr[i] >>= 1;
            m_arr[i] |= (m_arr[i + 1] & 0b1) ? top_bit : 0;
        }
        m_arr[backing_size - 1] >>= 1;
    }

    constexpr void print() {
        printf("{");
        for (I64 i = backing_size - 1; i >= 0; --i) {
            printf("%016llx ", m_arr[i]);
        }
        printf("}\n");
    }

  private:
    /// Helpful mask for the top bit in a U64.
    static constexpr U64 top_bit = U64(1) << 63;
    
    /// Backing array.    
    U64 m_arr[backing_size];
      
};
