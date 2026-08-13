/**
 * @file LaunchpadDevice.h
 * @brief Declares the LaunchpadDevice component used by the sequencer controller.
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

#include "core/midi/MidiMessage.h"

#include <array>
#include <bitset>
#include <functional>

// Compatible with: Launchpad S, Launchpad Mini Mk1 and Mk2
/**
 * @brief Implements protocol and state handling for the launchpad device.
 */
class LaunchpadDevice {
public:
    /**
     * @brief Rows constant used by this component.
     */
    static constexpr int Rows = 8; ///< Number of physical/simulated button-matrix rows.
    /**
     * @brief Cols constant used by this component.
     */
    static constexpr int Cols = 8; ///< Number of columns in the main Launchpad pad grid.
    /**
     * @brief Extra rows constant used by this component.
     */
    static constexpr int ExtraRows = 2; ///< Number of non-pad control rows exposed in addition to the main Launchpad grid.
    static constexpr int ButtonCount = (Rows + ExtraRows) * Cols; ///< Total number of addressable Launchpad buttons across the main grid and extra rows. ///< Total number of addressable Launchpad buttons across the main grid and extra rows.

    /**
     * @brief Scene row constant used by this component.
     */
    static constexpr int SceneRow = 8; ///< Launchpad row index assigned to scene controls.
    /**
     * @brief Function row constant used by this component.
     */
    static constexpr int FunctionRow = 9; ///< Launchpad row index assigned to global/function controls.

    typedef std::function<bool(uint8_t cable, const MidiMessage &)> SendMidiHandler;
    typedef std::function<void(int, int, bool)> ButtonHandler;

    /**
     * @brief Stores the fields required to represent color.
     */
    struct Color {
        /**
         * @brief Packs Launchpad LED color components into the device byte representation.
         */
        union {
            struct {
                uint8_t red: 4;
                uint8_t green: 4;
            };
            uint8_t data;
        };

        /**
         * @brief Constructs a Color instance.
         *
         * @param[in] red Red color-channel intensity.
         * @param[in] green Green color-channel intensity.
         */
        Color(int red, int green) : red(red), green(green) {}
    };

    /**
     * @brief Constructs a LaunchpadDevice instance.
     */
    LaunchpadDevice();
    /**
     * @brief Destroys the LaunchpadDevice instance.
     */
    virtual ~LaunchpadDevice();

    // midi handling

    /**
     * @brief Sets the send midi handler.
     *
     * @param[in] sendMidiHandler Callback invoked when the associated operation or event occurs.
     */
    void setSendMidiHandler(SendMidiHandler sendMidiHandler) {
        _sendMidiHandler = sendMidiHandler;
    }

    /**
     * @brief Receives a MIDI message from the platform transport.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    virtual void recvMidi(uint8_t cable, const MidiMessage &message);

    // initialization

    /**
     * @brief Initializes the LaunchpadDevice state.
     */
    virtual void initialize() {}

    // button handling

    /**
     * @brief Sets the button handler.
     *
     * @param[in] buttonHandler Callback invoked when the associated operation or event occurs.
     */
    void setButtonHandler(ButtonHandler buttonHandler) {
        _buttonHandler = buttonHandler;
    }

    /**
     * @brief Returns button state.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     *
     * @return `true` if button state; otherwise `false`.
     */
    bool buttonState(int row, int col) const {
        return _buttonState[row * Cols + col];
    }

    // led handling

    /**
     * @brief Clears leds.
     */
    void clearLeds() {
        std::fill(_ledState.begin(), _ledState.end(), 0);
    }

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    virtual void setLed(int row, int col, Color color) {
        _ledState[row * Cols + col] = color.data;
    }

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    virtual void setLed(int row, int col, int red, int green) {
        uint8_t state = (red & 0x3) | ((green & 0x3) << 4);
        _ledState[row * Cols + col] = state;
    }

    /**
     * @brief Synchronizes controller LEDs with the current UI and model state.
     */
    virtual void syncLeds();

protected:
    /**
     * @brief Cable constant used by this component.
     */
    static constexpr uint8_t Cable = 0; ///< USB-MIDI cable number used by this Launchpad device protocol.

    /**
     * @brief Sends a MIDI message through the requested output port.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if send midi; otherwise `false`.
     */
    bool sendMidi(uint8_t cable, const MidiMessage &message) {
        if (_sendMidiHandler) {
            return _sendMidiHandler(cable, message);
        }
        return false;
    }

    /**
     * @brief Sets the button state.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] state New state or logical level to apply.
     */
    void setButtonState(int row, int col, bool state) {
        _buttonState[row * Cols + col] = state;
        if (_buttonHandler) {
            _buttonHandler(row, col, state);
        }
    }

    SendMidiHandler _sendMidiHandler; ///< Callback invoked for send midi activity.
    ButtonHandler _buttonHandler; ///< Callback invoked for button activity.
    std::bitset<ButtonCount> _buttonState; ///< Current pressed/released state for every Launchpad button.
    /**
     * @brief Fixed-capacity storage for led state.
     */
    std::array<uint8_t, ButtonCount> _ledState; ///< Desired LED color/state for every Launchpad button.
    /**
     * @brief Fixed-capacity storage for device led state.
     */
    std::array<uint8_t, ButtonCount> _deviceLedState; ///< Last LED state sent to the physical Launchpad, used to suppress redundant MIDI updates.
};
