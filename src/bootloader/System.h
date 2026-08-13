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

/**
 * @brief Provides bootloader access to system ticks and reset/runtime services.
 */
class System {
public:
    /**
     * @brief Initializes the System and its runtime resources.
     */
    static void init();
    /**
     * @brief Releases runtime resources owned by the System.
     */
    static void deinit();

    /**
     * @brief Resets the System to its initial runtime state.
     */
    static void reset();

    /**
     * @brief Starts sys tick.
     */
    static void startSysTick();
    /**
     * @brief Stops sys tick.
     */
    static void stopSysTick();

    /**
     * @brief Returns the current absolute sequencer engine tick.
     */
    static void tick() { ++_ticks; }
    /**
     * @brief Returns the ticks.
     *
     * @return Current platform/simulator tick counter.
     */
    static uint32_t ticks() { return _ticks; }

private:
    /**
     * @brief Bootloader value representing ticks.
     */
    static uint32_t _ticks; ///< Monotonic platform tick counter maintained by the timer interrupt.
};
