#pragma once

#include "fifo.h"
#include "input_buffer.h"
#include <utils.h>
#include <ints.h>

constexpr U8 INPUT_BUFFER_DEPTH = 12;
constexpr U8 INPUT_BUFFER_WIDTH = 4;
constexpr U16 IMEM_SIZE_INSTS = 128;
constexpr U8 LAYER_ONE_DEPTH = 7;
constexpr U8 LAYER_TWO_DEPTH = 6;
constexpr U32 TX_FIFO_SIZE = 8;
constexpr U32 RX_FIFO_SIZE = 8;

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
    U8 m_data;

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
    U8 m_data;

public:
    /// Default constructor.
    constexpr L2Entry() : m_data(0) {};

    /// Raw constructor. Takes 6 bits.
    constexpr L2Entry(const U8 raw) : m_data(raw & 0b111111) {};

    /// Useful constructor.
    /// @param clear Whether to clear the counter.
    /// @param shift Whether to shift on this pattern.
    /// @param bits Bits to shift.
    constexpr L2Entry(const bool clear, const bool shift, const U8 bits) : m_data((clear ? 1<<5 : 0) | (shift ? 1<<4 : 0) | (bits & 0b1111)) {};

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

    /// Output buffer.
    InputBuffer<INPUT_BUFFER_DEPTH, INPUT_BUFFER_WIDTH> m_tx_serial;

    /// Layer 1 register.
    BitArray<LAYER_ONE_DEPTH> m_layer_one;

    /// Layer 2 register.
    BitArray<LAYER_TWO_DEPTH> m_layer_two;

    /// Instruction memory.
    U16 m_imem[IMEM_SIZE_INSTS];

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

    /// whether we're in clock mode.
    bool m_clock_mode;

    /// Whether uio[0] was previous high or low.
    bool m_prev_uio0 = false;

    /// TX fifo, filled by host.
    Fifo<TX_FIFO_SIZE> m_tx_fifo;

    /// RX fifo, consumed by host.
    Fifo<RX_FIFO_SIZE> m_rx_fifo;

    /// How many valid bits have shifted in.
    BitArray<LAYER_TWO_DEPTH> m_in_count;

    /// How many clocks we have waited on l1.
    U16 m_l1_count = 0;

    /// Whether layer 1 holds a new sample.
    bool m_l1_valid = false;

    /// How many bits to shift in from L2.
    U16 m_shift_count = 1;

    /// Simulate one step of this ASIC model. This assumes that the externally-visible pins are
    /// already set.   
    void tick() {
        if (!rst_n) {
            reset_all();
            return;
        }
        
        // Save old state.
        Model new_state = *this;

        new_state.m_pc = (m_pc + 1) & 0x7F;
        new_state.m_regs[RegAddr::TIME] =
            (m_regs[RegAddr::TIME] ? m_regs[RegAddr::TIME] - 1 : m_regs[RegAddr::TIME_RST]) & 0xFFF;

        // Clock mode shifts lanes 1-3 directly and bypasses layer 2.
        U8 shifted_lanes = update_layer1(new_state) ? 0xE : 0;
        if (!m_clock_mode) {
            bool shifted_into_layer2 = update_layer2(new_state);
            update_rx_buffer(new_state, shifted_into_layer2);
            if ((*m_in_count.GetStorage() & m_regs[RegAddr::ACTIVATION_MASK] & 0x3F) &&
                m_lut2[U16(*m_layer_two.GetStorage()) & 0x3F].IsShiftSet()) {
                shifted_lanes = 0x1;
            }
        }

        // Interrupts suppress the current instruction and resume it after RET.
        if (shifted_lanes && (m_regs[RegAddr::INT_ADDR] & 0x7F) && !m_int_running) {
            new_state.m_ret_add = m_pc;
            new_state.m_pc = m_regs[RegAddr::INT_ADDR] & 0x7F;
            new_state.m_int_running = true;
        } else {
            execute_instruction(new_state, (m_imem[m_pc] & 0xFFF), shifted_lanes);
        }

        // Update current state.
        *this = new_state;
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
        m_tx_serial.reset();
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
        m_clock_mode = false;
        m_prev_uio0 = false;
        m_in_count.ClearAll();
        m_l1_count = 0;
        m_l1_valid = false;
        m_shift_count = 1;
        m_tx_fifo.Clear();
        m_rx_fifo.Clear();
        ZERO_ARRAY(uio_out);
        ZERO_ARRAY(uio_oe);
        ZERO_ARRAY(u_out);
    }

    /// Private helper function to execute an instruction.
    void execute_instruction(Model& new_state, const U16 inst, const U8 shifted_lanes) const
    {
        // Four MSBs are opcode.
        const U16 opcode = (inst >> 8) & 0xF;
        const U16 opcode_second = (inst >> 6) & 0x3;
        const U8 imm8 = inst & 0xFF;
        const U8 imm4 = inst & 0x0F;
        const U8 imm2 = inst & 0x03;
        const U8 r1 = inst & 0x07;
        const bool msb_first = inst & 0x20;

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
                new_state.m_acc =
                    (!m_rx_fifo.CanPush() << 3) |
                    (!m_rx_fifo.CanPop() << 2) |
                    (!m_tx_fifo.CanPush() << 1) |
                    (!m_tx_fifo.CanPop() << 0);
                break;
                
                default:
                REPORT_ERROR("Invalid secondary opcode.");
                break;
            }
            break;

            case Opcodes::INST_ALU:
            switch (opcode_second) {
                case OpsALU::ALU_ADDU:
                new_state.m_acc = (m_acc + m_regs[r1]) & 0xFFF;
                break;

                case OpsALU::ALU_OR:
                new_state.m_acc = (m_acc | m_regs[r1]) & 0xFFF;
                break;

                case OpsALU::ALU_AND:
                new_state.m_acc = (m_acc & m_regs[r1]) & 0xFFF;
                break;

                case OpsALU::ALU_XOR:
                new_state.m_acc = (m_acc ^ m_regs[r1]) & 0xFFF;
                break;
            }
            break;

            case Opcodes::INST_NOT:
            new_state.m_acc = ~m_regs[r1] & 0xFFF;
            break;

            case Opcodes::INST_ADDI:
            new_state.m_acc = (m_acc + (imm8 & 0x80 ? imm8 - 0x100 : imm8)) & 0xFFF;
            break;

            case Opcodes::INST_ORI:
            new_state.m_acc = (m_acc | imm8) & 0xFFF;
            break;

            case Opcodes::INST_ANDI:
            new_state.m_acc = m_acc & imm8;
            break;

            case Opcodes::INST_XORI:
            new_state.m_acc = (m_acc ^ imm8) & 0xFFF;
            break;

            case Opcodes::INST_SHIFT:
            switch (opcode_second) {
                case OpsShift::SHIFT_RIGHT_REG:
                new_state.m_acc = m_regs[r1] < 12 ? (m_acc & 0xFFF) >> m_regs[r1] : 0;
                break;

                case OpsShift::SHIFT_RIGHT_IMM:
                new_state.m_acc = (m_acc & 0xFFF) >> imm4;
                break;

                case OpsShift::SHIFT_LEFT_REG:
                new_state.m_acc = m_regs[r1] < 12 ? (m_acc << m_regs[r1]) & 0xFFF : 0;
                break;

                case OpsShift::SHIFT_LEFT_IMM:
                new_state.m_acc = (m_acc << imm4) & 0xFFF;
                break;
            }
            break;

            case Opcodes::INST_INOUT:
            switch (opcode_second) {
                case OpsInOut::INOUT_IN:
                new_state.m_acc = 0;
                for (U64 i = 0; i < INPUT_BUFFER_DEPTH; ++i) {
                    const U64 bit = msb_first ? i : INPUT_BUFFER_DEPTH - 1 - i;
                    if (m_rx_serial.read_lane(imm2).IsBitSet(bit)) {
                        new_state.m_acc |= 1U << i;
                    }
                }
                break;

                case OpsInOut::INOUT_OUT:
                for (U64 i = 0; i < INPUT_BUFFER_DEPTH; ++i) {
                    const U64 bit = msb_first ? INPUT_BUFFER_DEPTH - 1 - i : i;
                    new_state.m_tx_serial.shift_into_lane(imm2, m_acc & (1U << bit));
                }
                break;

                case OpsInOut::INOUT_SHIN:
                if ((imm4 & 0x1) && !(shifted_lanes & 0x1)) {
                    new_state.m_rx_serial.shift_into_lane(0, uio_in[0]);
                }
                if ((imm4 & 0x2) && !(shifted_lanes & 0x2)) {
                    new_state.m_rx_serial.shift_into_lane(1, uio_in[1]);
                }
                if ((imm4 & 0x4) && !(shifted_lanes & 0x4)) {
                    new_state.m_rx_serial.shift_into_lane(2, uio_in[2]);
                }
                if ((imm4 & 0x8) && !(shifted_lanes & 0x8)) {
                    new_state.m_rx_serial.shift_into_lane(3, uio_in[3]);
                }
                break;

                case OpsInOut::INOUT_SHOUT:
                for (U64 i = 0; i < INPUT_BUFFER_WIDTH; ++i) {
                    if (imm4 & (1U << i)) {
                        new_state.uio_out[i] = m_tx_serial.read_lane(i).IsBitSet(INPUT_BUFFER_DEPTH - 1);
                        new_state.m_tx_serial.shift_into_lane(i, false);
                    }
                }
                break;
            }
            break;

            case Opcodes::INST_LOADI:
            new_state.m_acc = imm8;
            break;

            case Opcodes::INST_PUSH_POP_R:
            switch (opcode_second & 0x2) {
                case OpsPushPopR::PUSHPOPR_PUSHR:
                new_state.m_regs[r1] = m_acc & 0xFFF;
                break;

                case OpsPushPopR::PUSHPOPR_POPR:
                new_state.m_acc = m_regs[r1] & 0xFFF;
                break;
            }
            break;

            case Opcodes::INST_PUSH_POP:
            switch (opcode_second & 0x2) {
                case OpsPushPop::PUSHPOP_POP_TX:
                if (m_tx_fifo.CanPop()) {
                    new_state.m_acc = new_state.m_tx_fifo.Pop();
                }
                break;

                case OpsPushPop::PUSHPOP_PUSH_RX:
                new_state.m_rx_fifo.Push(m_acc & 0xFF);
                break;
            }
            break;

            case Opcodes::INST_JMP:
            new_state.m_pc = inst & 0x7F;
            break;

            case Opcodes::INST_JZERO:
            if (m_acc == 0) {
                new_state.m_pc = inst & 0x7F;
            }
            break;

            case Opcodes::INST_JNEG:
            if (m_acc & 0x800) {
                new_state.m_pc = inst & 0x7F;
            }
            break;

            case Opcodes::INST_RET:
            new_state.m_pc = m_ret_add;
            new_state.m_int_running = false;
            break;
        }
    }

    /// Update layer 1. Returns whether clock mode shifted RX lanes 1-3.
    bool update_layer1(Model& new_state) {
        new_state.m_l1_valid = false;
        if (m_l1_count) {
            new_state.m_l1_count = m_l1_count - 1;
            return false;
        }
        new_state.m_l1_count = m_regs[RegAddr::RX_DIVIDER] & 0xFFF;
        new_state.m_prev_uio0 = uio_in[0];
        if (m_clock_mode) {
            if (uio_in[0] && !m_prev_uio0) {
                new_state.m_rx_serial.shift_into_lane(1, uio_in[1]);
                new_state.m_rx_serial.shift_into_lane(2, uio_in[2]);
                new_state.m_rx_serial.shift_into_lane(3, uio_in[3]);
                return true;
            }
            return false;
        }
        new_state.m_layer_one.ShiftUp(uio_in[0]);
        new_state.m_l1_valid = true;
        return false;
    }

    /// Private helper to update layer 2. Returns whether l2 was updated or not.
    bool update_layer2(Model& new_state) {
        if (!m_l1_valid) {
            return false;
        }
        const U16 l1_reg = U16(*m_layer_one.GetStorage()) & 0x7F;
        const L1Entry& entry = m_lut1[l1_reg];
        if (entry.IsShiftSet()) {
            new_state.m_layer_two.ShiftUp(entry.ShiftVal());
            new_state.m_in_count.ShiftUp(0x1);
            return true;
        }
        return false;
    }

    /// Private helper to update RX buffer.
    void update_rx_buffer(Model& new_state, const bool shifted_in) {
        const U16 l2_reg = U16(*m_layer_two.GetStorage()) & 0x3F;
        const L2Entry& entry = m_lut2[l2_reg];
        if (*m_in_count.GetStorage() & m_regs[RegAddr::ACTIVATION_MASK] & 0x3F) {
            for (U64 i = m_shift_count; entry.IsShiftSet() && i > 0; --i) {
                new_state.m_rx_serial.shift_into_lane(0, entry.GetBits() & (1U << (i - 1)));
            }
            if (entry.IsClearSet()) {
                // Clear in_count, but preserve the new bit from l1.
                new_state.m_in_count.ClearAll();
                new_state.m_in_count.ShiftUp(shifted_in);
            }
        }
    }
};
