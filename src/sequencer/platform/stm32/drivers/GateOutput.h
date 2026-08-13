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

/**
 * @brief Provides the gate output hardware/platform abstraction.
 */
class GateOutput {
public:
    /**
     * @brief Constructs a GateOutput instance.
     *
     * @param[in] shiftRegister Shift-register identifier or state used by the hardware interface.
     */
    GateOutput(ShiftRegister &shiftRegister);

    /**
     * @brief Initializes the GateOutput and its runtime resources.
     */
    void init();

    /**
     * @brief Updates the GateOutput for the current service cycle.
     */
    void update();

    /**
     * @brief Returns the gates.
     *
     * @return Current bit mask/state of gate outputs.
     */
    inline uint8_t gates() const { return _gates; }

    /**
     * @brief Sets the gates.
     *
     * @param[in] gates Gate bit mask or gate-state collection consumed by the operation.
     */
    inline void setGates(uint8_t gates) {
        _gates = gates;
    }

    /**
     * @brief Sets the gate.
     *
     * @param[in] index Zero-based gate index.
     * @param[in] value New gate to store or apply.
     */
    inline void setGate(int index, bool value) {
        if (value) {
            _gates |= (1 << index);
        } else {
            _gates &= ~(1 << index);
        }
    }

private:
    /**
     * @brief Reference to shift register owned by another component.
     */
    ShiftRegister &_shiftRegister; ///< Reference to shift register owned by another component.
    /**
     * @brief Bit mask containing current gate-output states.
     */
    uint8_t _gates = 0; ///< Bit mask containing current gate-output states.
};
