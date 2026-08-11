/**
 * @file System.h
 * @brief Declares the System component used by the bootloader.
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
public:
    static void init();
    static void deinit();

    static void reset();

    static void startSysTick();
    static void stopSysTick();

    static void tick() { ++_ticks; }
    static uint32_t ticks() { return _ticks; }

private:
    static uint32_t _ticks;
};
