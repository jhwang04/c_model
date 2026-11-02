#pragma once

#include "input_buffer.h"
#include <ints.h>

constexpr U8 INPUT_BUFFER_DEPTH = 8;
constexpr U32 IMEM_SIZE_BYTES = 1024;
constexpr U32 DMEM_SIZE_BYTES = 256;

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
    InputBuffer8<INPUT_BUFFER_DEPTH> in_buffer;

    /// Instruction memory.
    U8 inst_mem[IMEM_SIZE_BYTES];

    /// Data memory.
    U8 data_mem[DMEM_SIZE_BYTES];

    /// Instruction pointer.
    U16 ip;

    /// Simulate one step of this ASIC model.    
    void tick() {
        if (!rst_n) {
            reset_all();
            return;
        }
    }

    /// Set instruction memory.
    void load_imem(U8 (&input_mem)[IMEM_SIZE_BYTES]) {
        for (U64 i = 0; i < IMEM_SIZE_BYTES; ++i) {
            inst_mem[i] = input_mem[i];
        }
    }
    
  private:
    /// Private helper function to reset all internal registers.
    void reset_all() {
        in_buffer.reset();        
    }
};
