/**
 * @file LaunchpadProMk3Device.h
 * @brief Declares the LaunchpadProMk3Device component used by the sequencer controller.
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

#include "LaunchpadDevice.h"

#include "core/midi/MidiMessage.h"

// Compatible with Launchpad Pro MK3
/**
 * @brief Implements protocol and state handling for the launchpad pro mk3 device.
 */
class LaunchpadProMk3Device : public LaunchpadDevice {
public:
    /**
     * @brief Constructs a LaunchpadProMk3Device instance.
     */
    LaunchpadProMk3Device();

    /**
     * @brief Initializes the LaunchpadProMk3Device state.
     */
    void initialize() override;

    /**
     * @brief Receives a MIDI message from the platform transport.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    void recvMidi(uint8_t cable, const MidiMessage &message) override;

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setLed(int row, int col, Color color) override {
        _ledState[row * Cols + col] = mapColor(color.red, color.green);
    }

    /**
     * @brief Sets the led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    void setLed(int row, int col, int red, int green) override {
        _ledState[row * Cols + col] = mapColor(red, green);;
    }

    /**
     * @brief Synchronizes controller LEDs with the current UI and model state.
     */
    void syncLeds() override;

private:
    /**
     * @brief Cable constant used by this component.
     */
    static constexpr uint8_t Cable = 0; ///< USB-MIDI cable number used by this Launchpad device protocol.

    /**
     * @brief Maps color.
     *
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     *
     * @return Mapped output/display color.
     */
    inline uint8_t mapColor(int red, int green) const {
        static const uint8_t map[] = {
        //  g0 g1 g2 g3
            0, 23, 22, 21, // r0
            7, 15, 18, 21, // r1
            6, 10, 14, 17, // r2
            5,  5,  9, 13, // r3
        };
        return map[(red & 0x3) * 4 + (green & 0x3)];
    }
};
