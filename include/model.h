#pragma once

#include "input_buffer.h"
#include <utils.h>
#include <ints.h>

constexpr U8 INPUT_BUFFER_DEPTH = 12;
constexpr U8 INPUT_BUFFER_WIDTH = 4;
constexpr U16 IMEM_SIZE_INSTS = 128;
constexpr U8 LAYER_ONE_DEPTH = 7;
constexpr U8 LAYER_TWO_DEPTH = 6;

/// Enum to index into register file.
enum RegAddr {
    INT_ADDR = 0,
    TIME = 1,
    TIME_RST = 2,
    RX_DIVIDER = 3,
    ACTIVATION_MASK = 4,
    GPR0 = 5,
    GPR1 = 6,
    GPR2 = 7,
    NUM_REGS = 8
};

/// Layer one entry.
struct L1Entry {

private:
    /// Backing data structure.
    const U8 m_data;

public:
    /// Default constructor.
    constexpr L1Entry() : m_data(0) {};

    /// Useful constructor.
    /// @param shift indicates whether or not the buffer should shift on a rising edge.
    /// @param value indicates whether to shift a one or a zero.
    constexpr L1Entry(const bool shift, const bool value) : m_data((shift ? 2 : 0) | (value ? 1 : 0)) {};

    /// Raw constructor. Takes the two LSBs.
    constexpr L1Entry(const U8 raw) : m_data(raw & 0b11) {}

    /// Getter.
    constexpr bool IsShiftSet() const {return m_data & 0b10;}

    // /// Setter.
    // constexpr void SetShift(const bool shift) {
    //     if (shift) {
    //         m_data |= 0b10;
    //     } else {
    //         m_data &= ~0b10;
    //     }
    // }

    /// Getter.
    constexpr U8 ShiftVal() const {return m_data & 0b1;}

    // /// Setter.
    // constexpr U8 SetShiftVal(const U8 val) {
    //     if (val) {
    //         m_data |= 0b01;
    //     } else {
    //         m_data &= ~0b01;
    //     }
    // }
};

/// Layer two entry.
struct L2Entry {
private:
    /// Backing data structure.
    const U8 m_data;

public:
    /// Default constructor.
    constexpr L2Entry() : m_data(0) {};

    /// Raw constructor. Takes 6 bits.
    constexpr L2Entry(const U8 raw) : m_data(raw & 0b111111) {};

    /// Useful constructor.
    /// @param clear Whether to clear the counter.
    /// @param shift Whether to shift on this pattern.
    /// @param bits Bits to shift.
    constexpr L2Entry(const bool clear, const bool shift, const U8 bits) : m_data(clear ? 1<<5 : 0 | shift ? 1<<4 : 0 | bits & 0b1111) {};

    /// Getters.
    constexpr bool IsClearSet() const {return m_data & 1<<5;}
    constexpr bool IsShiftSet() const {return m_data & 1<<4;}
    constexpr U8 GetBits() const {return m_data & 0b1111;}
};

/// Struct containing the current state of the model. In theory, all nets should
/// be computable combinationally from the bits stored here.
struct Model {
    /// ---------- EXTERNAL PINS ---------- ///
    /// Input pins.
    U8 ui_in[8];

    /// Input/Output pins. Only either uio_in or uio_out is valid at a time, depending
    /// on the valid of uio_oe (output enable).
    U8 uio_in[8];
    U8 uio_out[8];
    U8 uio_oe[8];    

    /// Output pins. These are editable externally.
    U8 u_out[8];

    /// External reset pin.
    bool rst_n = false;



    /// ---------- INTERNAL REGISTERS ---------- ///    
    /// Input buffer.
    InputBuffer<INPUT_BUFFER_DEPTH, INPUT_BUFFER_WIDTH> rx_serial;

    /// Layer 1 register.
    BitArray<LAYER_ONE_DEPTH> layer_one;

    /// Layer 2 register.
    BitArray<LAYER_TWO_DEPTH> layer_two;

    /// Instruction memory.
    U8 imem[IMEM_SIZE_INSTS];

    /// Instruction pointer.
    U16 regs[RegAddr::NUM_REGS];

    /// LUT for layer 1.
    L1Entry lut1[1 <<  LAYER_ONE_DEPTH];

    /// LUT for layer 2.
    L2Entry lut2[1 << LAYER_TWO_DEPTH];

    /// Program counter.
    U16 pc;

    /// Return address.
    U16 ret_add;

    /// Whether an interrupt is running.
    bool int_running;

    /// Simulate one step of this ASIC model. This assumes that the externally-visible pins are
    /// already set.   
    void tick() {
        if (!rst_n) {
            reset_all();
            return;
        }
    }

    /// Set instruction memory.
    void load_imem(U16 (&input_mem)[IMEM_SIZE_INSTS]) {
        for (U64 i = 0; i < IMEM_SIZE_INSTS; ++i) {
            imem[i] = input_mem[i];
        }
    }
    
  private:
    /// Private helper function to reset all internal registers.
    void reset_all() {
        rx_serial.reset();
        layer_one.ClearAll();
        layer_two.ClearAll();
        ZERO_ARRAY(imem);
        ZERO_ARRAY(regs);
        ZERO_ARRAY(lut1);
        ZERO_ARRAY(lut2);
        pc = 0;
        ret_add = 0;
        int_running = false;
    }
};
