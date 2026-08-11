/**
 * @file GateOutput.h
 * @brief Declares the GateOutput component used by the STM32 sequencer driver.
 *
 * @author Simon Kallweit — original PER|FORMER implementation lineage
 * @author Axel Napolitano — Styr modifications, integration and modernization
 * @copyright 2017-2018 Simon Kallweit
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "ShiftRegister.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput(ShiftRegister &shiftRegister);

    void init();

    void update();

    inline uint8_t gates() const { return _gates; }

    inline void setGates(uint8_t gates) {
        _gates = gates;
    }

    inline void setGate(int index, bool value) {
        if (value) {
            _gates |= (1 << index);
        } else {
            _gates &= ~(1 << index);
        }
    }

private:
    ShiftRegister &_shiftRegister;
    uint8_t _gates = 0;
};
