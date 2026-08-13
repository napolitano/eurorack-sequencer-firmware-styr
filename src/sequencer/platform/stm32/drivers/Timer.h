/**
 * @file Timer.h
 * @brief Declares the Timer component used by the STM32 sequencer driver.
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

#include <functional>

/**
 * @brief Provides the timer hardware/platform abstraction.
 */
class Timer {
public:
    /**
     * @brief Enumerates the supported hardware timer values.
     */
    enum HardwareTimer {
        // HardwareTimer1,
        // HardwareTimer2,
        HardwareTimer3, ///< Selects the hardware timer3 hardware timer.
        HardwareTimer4, ///< Selects the hardware timer4 hardware timer.
        HardwareTimer5, ///< Selects the hardware timer5 hardware timer.
        // HardwareTimer6,
        HardwareTimerCount, ///< Selects the hardware timer count hardware timer.
    };

    /**
     * @brief Constructs a Timer instance.
     *
     * @param[in] hardwareTimer STM32 hardware-timer identifier used by the driver.
     */
    Timer(HardwareTimer hardwareTimer);

    /**
     * @brief Initializes the Timer and its runtime resources.
     */
    void init();

    /**
     * @brief Resets the Timer to its initial runtime state.
     */
    void reset();
    /**
     * @brief Enables the Timer or feature.
     */
    void enable();
    /**
     * @brief Disables the Timer or feature.
     */
    void disable();

    /**
     * @brief Sets the period.
     *
     * @param[in] us Us, in microseconds.
     */
    void setPeriod(uint32_t us);

    /**
     * @brief Sets the handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setHandler(std::function<void()> handler);

private:
    HardwareTimer _hardwareTimer; ///< Low-level STM32 timer instance that drives this higher-level timer wrapper.
    /**
     * @brief Returns the stored callable or opaque platform value.
     *
     * @return The void value.
     */
    std::function<void()> _handler; ///< Callback invoked from the timer interrupt/service routine.
};
