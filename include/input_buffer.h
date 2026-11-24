#pragma once

#include <bitarray.h>
#include <ints.h>

/// Helper struct to store an 8-bit wide memory. Templated with the depth of the
/// buffer.
template <U64 depth, U64 width> class InputBuffer {
  public:
    static_assert(width <= 64);

    /// Default zero the buffer.
    InputBuffer8() {
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

    /// Shift in a bit to a particular lane.
    constexpr void shift_into_lane(U64 lane, bool value) { m_buf[lane].ShiftUp(value); }

    /// Shift in a whole row.
    constexpr void shift_word(U64 word) {
        U64 mask = 1;
        for (int i = 0; i < width; ++i) {
            m_buf[i].ShiftUp(word & mask);
            mask <<= 1;
        }
    }

    /// Reads one full lane. Does not allow mutation.
    const BitArray<depth>& read_lane(U64 lane) const { return m_buf[lane]; }

    /// Reads a full word.
    const U8 read_word(U64 i) {
        U8 out = 0;
        for (U64 lane = 0; lane < width; ++lane) {
            out <<= 1;            
            out |= m_buf[width - lane - 1].IsBitSet(i) ? 1 : 0;
        }
        return out;
    }

    /// Prints to stdout.
    void print() {
        printf("InputBuffer8: Width=8, Depth=%llu\n", depth);
        printf("IN ->     Buffer     -> BACK\n");
        for (U64 l = 0; l < width; ++l) {
            BitArray<depth>& lane = m_buf[l];
            printf("Lane %02llu: ", l);
            for (U64 row = 0; row < depth; ++row) {
                printf("%d ", lane.IsBitSet(row) ? 1 : 0);
            }
            printf("\n");
        }
    }

  private:
    /// Backing data structure for the two dimensional fifo.
    BitArray<depth> m_buf[width];
};

using InputBuffer4 = InputBuffer<12, 4>;