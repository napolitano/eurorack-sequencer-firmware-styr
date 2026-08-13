/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the bootloader.
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

/**
 * @brief Reads and classifies rotary-encoder rotation and push-button transitions.
 */
class Encoder {
public:
    /**
     * @brief Initializes the Encoder and its runtime resources.
     */
    static void init();
    /**
     * @brief Releases runtime resources owned by the Encoder.
     */
    static void deinit();

    /**
     * @brief Processes the supplied input for this component.
     */
    static void process();

    /**
     * @brief Returns the down.
     *
     * @return `true` if down; otherwise `false`.
     */
    static bool down() { return _down; }

    /**
     * @brief Resets the Encoder to its initial runtime state.
     */
    static void reset();
    /**
     * @brief Returns the pressed.
     *
     * @return `true` if pressed; otherwise `false`.
     */
    static bool pressed();
    /**
     * @brief Returns the released.
     *
     * @return `true` if released; otherwise `false`.
     */
    static bool released();
    /**
     * @brief Returns the value.
     *
     * @return Value carried by this event or model field.
     */
    static int value();

private:
    static bool _down; ///< Whether the encoder/button input is currently held down.
    static bool _pressed; ///< Whether a press edge was detected during the current scan.
    static bool _released; ///< Whether a release edge was detected during the current scan.
    /**
     * @brief Bootloader value representing value.
     */
    static int _value; ///< Accumulated encoder position reported by the bootloader input driver.
};
