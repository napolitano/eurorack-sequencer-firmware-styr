/**
 * @file ClockTimer.h
 * @brief Declares the ClockTimer component used by the STM32 sequencer driver.
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
 * @brief Provides the clock timer hardware/platform abstraction.
 */
class ClockTimer {
public:
    /**
     * @brief Defines callbacks emitted by the surrounding component.
     */
    struct Listener {
        /**
         * @brief Handles the clock timer tick callback.
         */
        virtual void onClockTimerTick() = 0;
    };

    /**
     * @brief Initializes the ClockTimer and its runtime resources.
     */
    void init();

    /**
     * @brief Resets the ClockTimer to its initial runtime state.
     */
    void reset();
    /**
     * @brief Enables the ClockTimer or feature.
     */
    void enable();
    /**
     * @brief Disables the ClockTimer or feature.
     */
    void disable();

    /**
     * @brief Returns the period.
     *
     * @return Configured timer period in microseconds.
     */
    uint32_t period() const { return _period; }
    /**
     * @brief Sets the period.
     *
     * @param[in] us Us, in microseconds.
     */
    void setPeriod(uint32_t us);

    /**
     * @brief Sets the listener.
     *
     * @param[in] listener Listener that receives callbacks; `nullptr` detaches the listener where supported.
     */
    void setListener(Listener *listener);

private:
    /**
     * @brief Driver value representing period.
     */
    uint32_t _period = 0; ///< Configured timer period in microseconds.
};
