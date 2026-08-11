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

class DebugLed {
    uint32_t _port;
    uint16_t _pin;

public:
    DebugLed(uint32_t port, uint16_t pin) :
        _port(port),
        _pin(pin)
    {
        gpio_mode_setup(_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, _pin);
    }

    void set(bool value) {
        if (value) {
            on();
        } else {
            off();
        }
    }

    void on() {
        gpio_set(_port, _pin);
    }

    void off() {
        gpio_clear(_port, _pin);
    }

};
