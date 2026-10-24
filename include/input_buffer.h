#pragma once

#include <bitarray.h>
#include <ints.h>

/// Helper struct to store an 8-bit wide memory. Templated with the depth of the
/// buffer.
template <U64 depth> class InputFifo {

    static constexpr U32 FIFO_WIDTH = 8;
    static_assert(FIFO_WIDTH % 8 == 0);
    static_assert(FIFO_WIDTH <= 64);
    static_assert(depth <= 64);

    /// Default zero the buffer.
    InputFifo() {
        for (U64 i = 0; i < FIFO_WIDTH; ++i) {
            m_buf[i].ClearAll();
        }
    }

    /// Re-zero the buffer when reset.
    constexpr void reset() {
        for (U64 i = 0; i < FIFO_WIDTH; ++i) {
            m_buf[i].ClearAll();
        }
    }

    /// Shift in a bit to a particular lane.
    constexpr void shift_into_lane(U64 lane, bool value) { m_buf[lane].ShiftUp(value); }

    /// Shift in a whole row.
    constexpr void shift_word(U64 word) {
        U64 mask = 1;
        for (int i = 0; i < FIFO_WIDTH; ++i) {
            m_buf[i].ShiftUp(word & mask);
            mask <<= 1;
        }
    }

    /// Read a full lane.
    const BitArray<depth> get_lane(U64 lane) { return m_buf[lane]; }

  private:
    /// Backing data structure for the two dimensional fifo.
    BitArray<depth> m_buf[FIFO_WIDTH];
};
