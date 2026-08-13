/**
 * @file Dio.h
 * @brief Declares the Dio component used by the STM32 sequencer driver.
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

#include <functional>

/**
 * @brief Provides the dio hardware/platform abstraction.
 */
class Dio {
public:
    /**
     * @brief Provides the input hardware/platform abstraction.
     */
    template<uint32_t Port, uint32_t Pin>
    /**
     * @brief Provides the input hardware/platform abstraction.
     */
    struct Input {
        typedef std::function<void(bool)> Handler;

        /**
         * @brief Initializes the Input and its runtime resources.
         */
        void init() {
            gpio_mode_setup(Port, GPIO_MODE_INPUT, GPIO_PUPD_NONE, Pin);
            update();
        }

        /**
         * @brief Returns the current digital input level.
         *
         * @return `true` when the digital input is logically active; otherwise `false`.
         */
        bool get() const {
            return _state;
        }

        /**
         * @brief Sets the handler.
         *
         * @param[in] handler Callback invoked when the associated event occurs.
         */
        void setHandler(Handler handler) { _handler = handler; }

        /**
         * @brief Handles the hardware/platform interrupt for this driver.
         */
        void interrupt() {
            update();
            if (_handler) {
                _handler(_state);
            }
        }

    private:
        /**
         * @brief Updates the Input for the current service cycle.
         */
        void update() {
            _state = !gpio_get(Port, Pin);
        }

        /**
         * @brief Whether state is true in the current state.
         */
        bool _state = false; ///< Last logical output state written to this STM32 digital-output pin.
        Handler _handler; ///< Callback handler invoked by this component.
    };

    /**
     * @brief Provides the output hardware/platform abstraction.
     */
    template<uint32_t Port, uint32_t Pin>
    /**
     * @brief Provides the output hardware/platform abstraction.
     */
    struct Output {
        /**
         * @brief Initializes the Output and its runtime resources.
         */
        void init() {
            gpio_mode_setup(Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, Pin);
        }

        /**
         * @brief Stores the supplied value.
         *
         * @param[in] value Logical/state value to store or drive.
         */
        void set(bool value) {
            if (value) {
                gpio_clear(Port, Pin);
            } else {
                gpio_set(Port, Pin);
            }
        }
    };

    /**
     * @brief Constructs a Dio instance.
     */
    Dio();

    /**
     * @brief Initializes the Dio and its runtime resources.
     */
    void init();

    Input<GPIOB, GPIO11> clockInput; ///< Digital input wrapper bound to the hardware clock input line.
    Input<GPIOB, GPIO10> resetInput; ///< Digital input wrapper bound to the hardware reset input line.

    Output<GPIOB, GPIO2> clockOutput; ///< Digital output wrapper bound to the hardware clock output line.
    Output<GPIOB, GPIO1> resetOutput; ///< Digital output wrapper bound to the hardware reset output line.
};
