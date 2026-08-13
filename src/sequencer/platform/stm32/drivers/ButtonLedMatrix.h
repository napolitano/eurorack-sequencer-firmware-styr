/**
 * @file ButtonLedMatrix.h
 * @brief Declares the ButtonLedMatrix component used by the STM32 sequencer driver.
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

#include "core/utils/RingBuffer.h"
#include "core/utils/Debouncer.h"

#include "drivers/ShiftRegister.h"

#include <array>
#include <utility>

#include <cstdint>
#include <cstdlib>

/**
 * @brief Provides the button led matrix hardware/platform abstraction.
 */
class ButtonLedMatrix {
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
     *
     * @param[in] shiftRegister Shift-register identifier or state used by the hardware interface.
     * @param[in] invertLeds Whether invert leds is enabled for this operation.
     */
    ButtonLedMatrix(ShiftRegister &shiftRegister, bool invertLeds = false);

    /**
     * @brief Initializes the ButtonLedMatrix and its runtime resources.
     */
    void init();

    /**
     * @brief Sets the led.
     *
     * @param[in] index Zero-based LED index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    void setLed(int index, uint8_t red, uint8_t green) {
        if (_invertLeds) {
            std::swap(red, green);
        }
        _ledState[index].red.intensity = red >> 4;
        _ledState[index].green.intensity = green >> 4;
        if (red == 0) {
            _ledState[index].red.counter = 0;
        }
        if (green == 0) {
            _ledState[index].green.counter = 0;
        }
    }

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    inline void setLed(int row, int col, uint8_t red, uint8_t green) {
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
    inline bool buttonState(int index) const {
        return _buttonState[index].state;
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
    void process();

    /**
     * @brief Returns next event.
     *
     * @param[in] event Event to process.
     *
     * @return `true` if next event; otherwise `false`.
     */
    inline bool nextEvent(Event &event) {
        if (_events.readable() < 1) {
            return false;
        }
        event = _events.read();
        return true;
    }

private:
    /**
     * @brief Provides the led hardware/platform abstraction.
     */
    struct Led {
        /**
         * @brief Driver value representing intensity.
         */
        uint8_t intensity : 4; ///< Four-bit LED intensity requested for this matrix cell.
        /**
         * @brief Driver value representing counter.
         */
        uint8_t counter : 4; ///< Four-bit PWM phase counter used to realize LED intensity.
        /**
         * @brief Updates the Led for the current service cycle.
         *
         * @return `true` if update; otherwise `false`.
         */
        inline bool update() {
            // return true;
            uint8_t sum = counter + intensity;
            bool active = sum >= 0x0f;
            counter = sum & 0x0f;
            return active;
        };
    };

    /**
     * @brief Stores led state used while the component is running.
     */
    struct LedState {
        Led red; ///< Per-cell red LED drive/PWM state.
        Led green; ///< Per-cell green LED drive/PWM state.
    /**
     * @brief Returns attribute.
     *
     * @note Includes an unnamed `(packed)` input parameter as declared by the inherited/interface signature.
     *
     * @return Decoded/configured attribute value.
     */
    } __attribute__((packed));

    /**
     * @brief Stores button state used while the component is running.
     */
    struct ButtonState {
        /**
         * @brief Driver value representing state.
         */
        uint8_t state; ///< Current encoded state of the matrix scan entry.
        // uint8_t counter : 7;
    };

    /**
     * @brief Reference to shift register owned by another component.
     */
    ShiftRegister &_shiftRegister; ///< Reference to shift register owned by another component.
    /**
     * @brief Whether invert leds is true in the current state.
     */
    bool _invertLeds; ///< True when the hardware LED drive polarity must be inverted.

    ButtonState _buttonState[Rows * ColsButton]; ///< Owned fixed-size collection of button state.
    LedState _ledState[Rows * ColsLed]; ///< Owned fixed-size collection of led state.

    RingBuffer<Event, 16> _events; ///< Fixed-capacity FIFO holding pending events.

    /**
     * @brief Driver value representing row.
     */
    uint8_t _row = 0; ///< Current matrix or grid row index.
};
