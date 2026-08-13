/**
 * @file ButtonLedMatrix.h
 * @brief Declares the ButtonLedMatrix component used by the simulator driver.
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

#include "SystemConfig.h"

#include "core/Simulator.h"

#include <deque>
#include <array>
#include <bitset>

#include <cstdint>

/**
 * @brief Provides the button led matrix hardware/platform abstraction.
 */
class ButtonLedMatrix : private sim::TargetInputHandler {
public:
    /**
     * @brief Provides the event hardware/platform abstraction.
     */
    struct Event {
        /**
         * @brief Enumerates the supported action values.
         */
        enum Action {
            KeyDown, ///< Selects the key down action.
            KeyUp, ///< Selects the key up action.
        };

        /**
         * @brief Constructs a Event instance.
         */
        Event() = default;
        /**
         * @brief Constructs a Event instance.
         *
         * @param[in] action Action identifier to execute.
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         */
        Event(Action action, int value) : _action(action), _value(value) {}

        /**
         * @brief Returns the action.
         *
         * @return Decoded action associated with the input event.
         */
        Action action() const { return Action(_action); }
        /**
         * @brief Returns the value.
         *
         * @return Value carried by this event or model field.
         */
        int value() const { return _value; }

    private:
        /**
         * @brief Driver value representing action.
         */
        uint8_t _action; ///< Encoded button-matrix action associated with this event.
        /**
         * @brief Driver value representing value.
         */
        int8_t _value; ///< Payload value associated with this event or control state.
    };

    /**
     * @brief Rows constant used by this component.
     */
    static constexpr int Rows = CONFIG_BLM_ROWS; ///< Number of physical/simulated button-matrix rows.
    /**
     * @brief Cols button constant used by this component.
     */
    static constexpr int ColsButton = CONFIG_BLM_COLS_BUTTON; ///< Number of button-input columns scanned by the matrix driver.
    /**
     * @brief Cols led constant used by this component.
     */
    static constexpr int ColsLed = CONFIG_BLM_COLS_LED; ///< Number of LED-output columns driven by the matrix driver.

    /**
     * @brief Constructs a ButtonLedMatrix instance.
     */
    ButtonLedMatrix() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    /**
     * @brief Destroys the ButtonLedMatrix instance.
     */
    ~ButtonLedMatrix() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the ButtonLedMatrix and its runtime resources.
     */
    void init() {}

    /**
     * @brief Sets the led.
     *
     * @param[in] index Zero-based LED index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    void setLed(int index, uint8_t red, uint8_t green) {
        _simulator.writeLed(index, red > 0, green > 0);
    }

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    void setLed(int row, int col, uint8_t red, uint8_t green) {
        setLed(col * Rows + row, red, green);
    }

    /**
     * @brief Sets the leds.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    void setLeds(const std::array<std::pair<uint8_t, uint8_t>, Rows * ColsLed> &leds) {
        for (size_t i = 0; i < leds.size(); ++i) {
            setLed(i, leds[i].first, leds[i].second);
        }
    }

    /**
     * @brief Returns button state.
     *
     * @param[in] index Zero-based button index.
     *
     * @return `true` if button state; otherwise `false`.
     */
    bool buttonState(int index) const {
        return _buttonState[index];
    }

    /**
     * @brief Returns button state.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     *
     * @return `true` if button state; otherwise `false`.
     */
    inline bool buttonState(int row, int col) const {
        return buttonState(col * Rows + row);
    }

    /**
     * @brief Processes the supplied input for this component.
     */
    void process() {}

    /**
     * @brief Returns next event.
     *
     * @param[in] event Event to process.
     *
     * @return `true` if next event; otherwise `false`.
     */
    bool nextEvent(Event &event) {
        if (_events.empty()) {
            return false;
        }
        event = _events.front();
        _events.pop_front();
        return true;
    }

private:
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based write button index.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    void writeButton(int index, bool pressed) override {
        if (index < 0 || index >= 37) {
            return;
        }
        if (_buttonState[index] != pressed) {
            _buttonState[index] = pressed;
            _events.emplace_back(pressed ? Event::KeyDown : Event::KeyUp, index);
        }
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    /**
     * @brief Pointer to button state; `nullptr` denotes that no object/resource is assigned.
     */
    std::bitset<Rows * ColsButton> _buttonState; ///< Pointer to button state; `nullptr` denotes that no object/resource is assigned.
    std::deque<Event> _events; ///< Queued simulated button transitions waiting to be consumed by the driver.
};
