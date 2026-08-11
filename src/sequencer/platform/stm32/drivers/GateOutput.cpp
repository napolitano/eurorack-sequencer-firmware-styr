/**
 * @file GateOutput.cpp
 * @brief Implements the GateOutput component used by the STM32 sequencer driver.
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
#include "GateOutput.h"

GateOutput::GateOutput(ShiftRegister &shiftRegister) :
    _shiftRegister(shiftRegister)
{}

void GateOutput::init() {
}

void GateOutput::update() {
    _shiftRegister.write(2, _gates);
}
