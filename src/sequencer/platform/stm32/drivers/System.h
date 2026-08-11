/**
 * @file System.h
 * @brief Declares the System component used by the STM32 sequencer driver.
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

#include <cstdint>

class System {
    static volatile uint32_t _ticks;

public:
    static void init();

    static void reset();

    static uint32_t ticks() { return _ticks; }
    static void tick();

    static void startWatchdog(uint32_t ms);
    static void resetWatchdog();
};
