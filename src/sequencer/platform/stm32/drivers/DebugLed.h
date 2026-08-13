/**
 * @file DebugLed.h
 * @brief Declares the DebugLed component used by the STM32 sequencer driver.
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

#include <libopencm3/stm32/gpio.h>

#include <cstdint>

/**
 * @brief Provides the debug led hardware/platform abstraction.
 */
class DebugLed {
    /**
     * @brief Driver value representing port.
     */
    uint32_t _port; ///< GPIO peripheral base address used to drive the debug LED.
    /**
     * @brief Driver value representing pin.
     */
    uint16_t _pin; ///< GPIO pin mask used to drive the debug LED.

public:
    /**
     * @brief Constructs a DebugLed instance.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] pin Hardware/platform pin identifier.
     */
    DebugLed(uint32_t port, uint16_t pin) :
        /**
         * @brief Returns the port.
         */
        _port(port),
        /**
         * @brief Returns the pin.
         */
        _pin(pin)
    {
        gpio_mode_setup(_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, _pin);
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] value Logical/state value to store or drive.
     */
    void set(bool value) {
        if (value) {
            on();
        } else {
            off();
        }
    }

    /**
     * @brief Sets the output/device state on.
     */
    void on() {
        gpio_set(_port, _pin);
    }

    /**
     * @brief Sets the output/device state off.
     */
    void off() {
        gpio_clear(_port, _pin);
    }

};
