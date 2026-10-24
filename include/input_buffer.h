#pragma once

#include <bitarray.h>
#include <ints.h>

/// Helper struct to store an 8-bit wide memory. Templated with the depth of the
/// buffer.
template <U64 width, U64 depth> class InputFifo {

    /// Default zero the buffer.
    InputFifo() {
        for (U64 i = 0; i < width; ++i) {
            m_buf[i].ClearAll();
        }
    }

    /// Re-zero the buffer when reset.
    constexpr void reset() {
        for (U64 i = 0; i < width; ++i) {
            m_buf[i].ClearAll();
        }
    }

  private:
    /// Backing data structure for the two dimensional fifo.
    BitArray<depth> m_buf[width];
};
