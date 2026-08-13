/**
 * @file HardwareConfig.h
 * @brief Declares the HardwareConfig component used by the hardware configuration firmware.
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/// @brief Flash address at which the hardware configuration record is stored.
#define HARDWARE_CONFIG_ADDR 0x08008000
/// @brief Magic value identifying a valid hardware configuration record.
#define HARDWARE_CONFIG_MAGIC 0x323f6e10
/// @brief Schema version of the stored hardware configuration record.
#define HARDWARE_CONFIG_VERSION 3

/**
 * @brief Enumerates the supported dac type values.
 */
enum DacType {
    DAC8568C, ///< Selects the dac8568 c dac type.
    DAC8568A ///< Selects the dac8568 a dac type.
};

/**
 * @brief Stores the versioned hardware-configuration bytes read by the firmware at startup.
 */
struct HardwareConfigData {
    /**
     * @brief Magic value used to identify a valid encoded structure/version tag.
     */
    uint32_t magic; ///< Magic value used to identify a valid encoded structure/version tag.
    /**
     * @brief Runtime value representing version.
     */
    uint32_t version; ///< Hardware-configuration structure version used to interpret the remaining fields.
    /**
     * @brief Runtime value representing reverse encoder.
     */
    uint8_t reverseEncoder; ///< Nonzero when the physical encoder direction is reversed.
    /**
     * @brief Runtime value representing invert leds.
     */
    uint8_t invertLeds; ///< Nonzero when the LED drive polarity is inverted.
    /**
     * @brief Runtime value representing dac type.
     */
    uint8_t dacType; ///< Encoded DAC hardware variant selected for the target board.
};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/**
 * @brief Stores and edits hardware config configuration.
 */
struct HardwareConfig {
    /**
     * @brief Returns the data.
     *
     * @return Reference to the data.
     */
    static const HardwareConfigData &data() { return *reinterpret_cast<const HardwareConfigData *>(HARDWARE_CONFIG_ADDR); }
    /**
     * @brief Reports whether valid.
     *
     * @return `true` if valid; otherwise `false`.
     */
    static bool isValid() { return data().magic == HARDWARE_CONFIG_MAGIC; }
    /**
     * @brief Returns the reverse encoder.
     *
     * @return `true` if reverse encoder; otherwise `false`.
     */
    static bool reverseEncoder() { return isValid() && data().reverseEncoder; }
    /**
     * @brief Returns the invert leds.
     *
     * @return `true` if invert leds; otherwise `false`.
     */
    static bool invertLeds() { return isValid() && data().version >= 2 && data().invertLeds; }
    /**
     * @brief Returns the dac type.
     *
     * @return Configured hardware DAC variant.
     */
    static DacType dacType() { return DacType(isValid() && data().version >= 3 ? data().dacType : 0); }
};

#endif
