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

/// Opcode enum.
enum Opcodes {
    INST_NOP_STATUS = 0x0,
    INST_ALU = 0x1,
    INST_NOT = 0x2,
    INST_ADDI = 0x3,
    INST_ORI = 0x4,
    INST_ANDI = 0x5,
    INST_XORI = 0x6,
    INST_SHIFT = 0x7,
    INST_INOUT = 0x8,
    INST_LOADI = 0x9,
    INST_PUSH_POP_R = 0xA,
    INST_PUSH_POP = 0xB,
    INST_JMP = 0xC,
    INST_JZERO = 0xD,
    INST_JNEG = 0xE,
    INST_RET = 0xF
};

/// Secondary opcode for NOP/Status instruction.
enum OpsNopStatus {
    NOP_STATUS_NOP = 0x0,
    NOP_STATUS_DIR = 0x1,
    NOP_STATUS_FIFO_STATUS = 0x2
};

/// Secondary opcodes for ALU instruction.
enum OpsALU {
    ALU_ADDU = 0x0,
    ALU_OR = 0x1,
    ALU_AND = 0x2,
    ALU_XOR = 0x3
};

/// Secondary opcodes for SHIFT instruction.
enum OpsShift {
    SHIFT_RIGHT_REG = 0x0,
    SHIFT_RIGHT_IMM = 0x1,
    SHIFT_LEFT_REG = 0x2,
    SHIFT_LEFT_IMM = 0x3
};

/// Secondary opcodes for INOUT instruction.
enum OpsInOut {
    INOUT_IN = 0x0,
    INOUT_OUT = 0x1,
    INOUT_SHIN = 0x2,
    INOUT_SHOUT = 0x3
};

/// Secondary opcodes for PUSHR/POPR instruction.
enum OpsPushPopR {
    PUSHPOPR_PUSHR = 0x0,
    PUSHPOPR_POPR = 0x2,
};

/// Secondary opcodes for PUSH/POP instruction.
enum OpsPushPop {
    PUSHPOP_POP_TX = 0x0,
    PUSHPOP_PUSH_RX = 0x2,
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
    InputBuffer<INPUT_BUFFER_DEPTH, INPUT_BUFFER_WIDTH> m_rx_serial;

    /// Layer 1 register.
    BitArray<LAYER_ONE_DEPTH> m_layer_one;

    /// Layer 2 register.
    BitArray<LAYER_TWO_DEPTH> m_layer_two;

    /// Instruction memory.
    U8 m_imem[IMEM_SIZE_INSTS];

    /// Instruction pointer.
    U16 m_regs[RegAddr::NUM_REGS];

    /// LUT for layer 1.
    L1Entry m_lut1[1 <<  LAYER_ONE_DEPTH];

    /// LUT for layer 2.
    L2Entry m_lut2[1 << LAYER_TWO_DEPTH];

    /// Program counter.
    U16 m_pc;

    /// Return address.
    U16 m_ret_add;

    // Accumulator.
    U16 m_acc;

    /// Whether an interrupt is running.
    bool m_int_running;

    /// Simulate one step of this ASIC model. This assumes that the externally-visible pins are
    /// already set.   
    void tick() {
        if (!rst_n) {
            reset_all();
            return;
        }
        
        // Save old state.
        Model new_state = *this;

        // Update CPU.
        const U16 curr_inst = m_imem[m_pc];
        execute_instruction(new_state, curr_inst);
        m_layer_one.ShiftUp(uio_in[0]);

        // Update current state.
        memcpy(this, &new_state, sizeof(Model));
    }

    /// Set instruction memory.
    void load_imem(U16 (&input_mem)[IMEM_SIZE_INSTS]) {
        for (U64 i = 0; i < IMEM_SIZE_INSTS; ++i) {
            m_imem[i] = input_mem[i];
        }
    }
    
  private:
    /// Private helper function to reset all internal registers.
    void reset_all() {
        m_rx_serial.reset();
        m_layer_one.ClearAll();
        m_layer_two.ClearAll();
        ZERO_ARRAY(m_imem);
        ZERO_ARRAY(m_regs);
        ZERO_ARRAY(m_lut1);
        ZERO_ARRAY(m_lut2);
        m_pc = 0;
        m_ret_add = 0;
        m_acc = 0;
        m_int_running = false;
    }

    /// Private helper function to execute an instruction.
    void execute_instruction(Model& new_state, const U16 inst) const
    {
        // Four MSBs are opcode.
        const U16 opcode = (inst >> 8) & 0xF;
        const U16 opcode_second = (inst >> 6) & 0x3;
        const U8 imm8 = inst & 0xFF;
        const U8 imm4 = inst & 0x0F;
        const U8 imm2 = inst & 0x03;
        const U8 r1 = inst & 0x07;

        switch (opcode) {
            case Opcodes::INST_NOP_STATUS:
            // Either NOP or status reg.
            switch (opcode_second) {
                case OpsNopStatus::NOP_STATUS_NOP:
                // Do nothing.
                break;

                case OpsNopStatus::NOP_STATUS_DIR:
                SET_REGISTER(new_state.uio_oe, 3, 0, (m_acc & 0xF));
                break;

                case OpsNopStatus::NOP_STATUS_FIFO_STATUS:
                U16 fifo_status = 0;
                
                new_state.m_acc =
                break;
                
                default:
                REPORT_ERROR("Invalid secondary opcode.");
                break;
            }
            break;

            case 0b0001:
            break;
        }
    }
};
