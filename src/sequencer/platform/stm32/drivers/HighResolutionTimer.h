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

/**
 * @brief Provides the high resolution timer hardware/platform abstraction.
 */
class HighResolutionTimer {
public:
    /**
     * @brief Initializes the HighResolutionTimer and its runtime resources.
     */
    static void init();

    /**
     * @brief Returns the us.
     *
     * @return Us, in microseconds.
     */
    static inline uint32_t us() {
        return _ticks;
    }

    /**
     * @brief Returns the current absolute sequencer engine tick.
     */
    static inline void tick() {
        ++_ticks;
    }

private:
    /**
     * @brief Driver value representing ticks.
     */
    static volatile uint32_t _ticks; ///< Monotonic platform tick counter maintained by the timer interrupt.
};
