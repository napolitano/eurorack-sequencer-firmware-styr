/**
 * @file Dio.h
 * @brief Declares the Dio component used by the simulator driver.
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

#include "core/Simulator.h"

#include <functional>
#include <memory>

/**
 * @brief Provides the dio hardware/platform abstraction.
 */
class Dio : private sim::TargetInputHandler {
public:
    /**
     * @brief Provides the input hardware/platform abstraction.
     */
    struct Input {
        typedef std::function<void(bool)> Handler;

        /**
         * @brief Returns the current digital input level.
         *
         * @return `true` when the digital input is logically active; otherwise `false`.
         */
        bool get() const { return _value; }
        /**
         * @brief Sets the handler.
         *
         * @param[in] handler Callback invoked when the associated event occurs.
         */
        void setHandler(Handler handler) { _handler = handler; }

    private:
        /**
         * @brief Stores the supplied value.
         *
         * @param[in] value Replacement value to store or apply.
         */
        void set(bool value) {
            if (_handler && value != _value) {
                _handler(value);
            }
            _value = value;
        }

        bool _value = false; ///< Whether value is true in the current state.
        Handler _handler; ///< Callback invoked when this digital signal changes.

        friend class Dio;
    };

    /**
     * @brief Provides the output hardware/platform abstraction.
     */
    struct Output {
        /**
         * @brief Stores the supplied value.
         *
         * @param[in] value Replacement value to store or apply.
         */
        void set(bool value) {
            if (_handler) {
                _handler(value);
            }
        }

    private:
        typedef std::function<void(bool)> Handler;

        /**
         * @brief Sets the handler.
         *
         * @param[in] handler Callback invoked when the associated event occurs.
         */
        void setHandler(Handler handler) { _handler = handler; }

        Handler _handler; ///< Callback invoked when this digital signal changes.

        friend class Dio;
    };

    /**
     * @brief Constructs a Dio instance.
     */
    Dio() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);

        clockOutput.setHandler([this] (int value) {
            _simulator.writeDigitalOutput(0, value);
        });

        resetOutput.setHandler([this] (int value) {
            _simulator.writeDigitalOutput(1, value);
        });
    }

    /**
     * @brief Destroys the Dio instance.
     */
    ~Dio() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the Dio and its runtime resources.
     */
    void init() {}

    Input clockInput; ///< Digital I/O endpoint for the clock input signal.
    Input resetInput; ///< Digital I/O endpoint for the reset input signal.

    Output clockOutput; ///< Digital I/O endpoint for the clock output signal.
    Output resetOutput; ///< Digital I/O endpoint for the reset output signal.

private:
    /**
     * @brief Writes digital input.
     *
     * @param[in] pin Pin used by the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeDigitalInput(int pin, bool value) override {
        switch (pin) {
        case 0: clockInput.set(value); break;
        case 1: resetInput.set(value); break;
        }
    }

    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
};
