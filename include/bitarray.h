#pragma once

#include <ints.h>
#include <stdio.h>

/// Helper class to store an array of bits compactly.
template <U64 n> class BitArray {
 public:  
    /// Guaranteed to be large enough. May have padding.
    static constexpr U64 backing_size = (n / 64) + 1;

    /// Public-facing constructor. Zero-inits backing array.
    /*    BitArray() {
        for (U64 i = 0; i < backing_size; ++i) {
            m_arr[i] = 0;
        }
        }*/
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
    constexpr void SetBit(U64 i) const {
        const U64 backing_i = i / 64;
        const U64 remainder = i % 64;
        const U64 mask = U64(1) << remainder;
        m_arr[backing_i] |= mask;
    }

  private:
    
    /// Backing array.    
    U64 m_arr[backing_size];
      
};
