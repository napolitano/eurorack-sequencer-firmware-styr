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

/**
 * @brief Provides the system hardware/platform abstraction.
 */
class System {
    /**
     * @brief Driver value representing ticks.
     */
    static volatile uint32_t _ticks; ///< Monotonic platform tick counter maintained by the timer interrupt.

public:
    /**
     * @brief Initializes the System and its runtime resources.
     */
    static void init();

    /**
     * @brief Resets the System to its initial runtime state.
     */
    static void reset();

    /**
     * @brief Returns the ticks.
     *
     * @return Current platform/simulator tick counter.
     */
    static uint32_t ticks() { return _ticks; }
    /**
     * @brief Returns the current absolute sequencer engine tick.
     */
    static void tick();

    /**
     * @brief Starts watchdog.
     *
     * @param[in] ms Ms, in milliseconds.
     */
    static void startWatchdog(uint32_t ms);
    /**
     * @brief Resets watchdog.
     */
    static void resetWatchdog();
};
