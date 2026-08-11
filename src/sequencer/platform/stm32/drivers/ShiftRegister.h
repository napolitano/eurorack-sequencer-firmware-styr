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

class ShiftRegister {
public:
    static constexpr int NumRegisters = CONFIG_NUM_SR;

    ShiftRegister();

    void init();

    void process();

    uint8_t read(int index) const { return _inputs[index]; }
    void write(int index, uint8_t value) { _outputs[index] = value; }

private:
    std::array<uint8_t, NumRegisters> _outputs;
    std::array<uint8_t, NumRegisters> _inputs;
};
