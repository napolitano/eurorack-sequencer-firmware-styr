/**
 * @file HighResolutionTimer.h
 * @brief Declares the HighResolutionTimer component used by the STM32 sequencer driver.
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

class HighResolutionTimer {
public:
    static void init();

    static inline uint32_t us() {
        return _ticks;
    }

    static inline void tick() {
        ++_ticks;
    }

private:
    static volatile uint32_t _ticks;
};
