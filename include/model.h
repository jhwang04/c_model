#pragma once

#include <ints.h>

static constexpr U8 INPUT_BUFFER_DEPTH = 8;

/// Struct containing the current state of the model. In theory, all nets should
/// be computable combinationally from the bits stored here.
struct Model {
    /// Input pins.
    U8 u_in[8];

    /// Input/Output pins.
    U8 u_io[8];

    /// Output pins. These are editable externally.
    U8 u_out[8];

    /// External reset pin.
    bool rst_n = false;
};
