/**
 * @file ShiftRegister.h
 * @brief Declares the ShiftRegister component used by the STM32 sequencer driver.
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

#include "SystemConfig.h"

#include <array>

#include <cstdint>

/**
 * @brief Provides the shift register hardware/platform abstraction.
 */
class ShiftRegister {
public:
    /**
     * @brief Num registers constant used by this component.
     */
    static constexpr int NumRegisters = CONFIG_NUM_SR; ///< Number of cascaded shift-register devices configured for this hardware.

    /**
     * @brief Constructs a ShiftRegister instance.
     */
    ShiftRegister();

    /**
     * @brief Initializes the ShiftRegister and its runtime resources.
     */
    void init();

    /**
     * @brief Processes the supplied input for this component.
     */
    void process();

    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] index Zero-based read index.
     *
     * @return Result of read().
     */
    uint8_t read(int index) const { return _inputs[index]; }
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] index Zero-based write index.
     * @param[in] value Value to write.
     */
    void write(int index, uint8_t value) { _outputs[index] = value; }

private:
    /**
     * @brief Fixed-capacity storage for outputs.
     */
    std::array<uint8_t, NumRegisters> _outputs; ///< Shadow bytes written to the cascaded output shift registers.
    /**
     * @brief Fixed-capacity storage for inputs.
     */
    std::array<uint8_t, NumRegisters> _inputs; ///< Most recently sampled bytes read from the cascaded input shift registers.
};
