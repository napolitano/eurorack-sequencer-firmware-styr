/**
 * @file Settings.h
 * @brief Declares the Settings component used by the sequencer model.
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

#include "Calibration.h"
#ifdef CONFIG_ADVANCED_SETTINGS
#include "AdvancedSettings.h"
#endif
#include "Serialize.h"

/**
 * @brief Stores and manipulates settings model data.
 */
class Settings {
public:
#ifdef CONFIG_ADVANCED_SETTINGS
    /**
     * @brief Version constant used by this component.
     */
    static constexpr uint32_t Version = 2; ///< Serialized settings schema version; advanced-settings builds use version 2 and standard builds use version 1.
#else
    /**
     * @brief Version constant used by this component.
     */
    static constexpr uint32_t Version = 1; ///< Serialized settings schema version; advanced-settings builds use version 2 and standard builds use version 1.
#endif

    /**
     * @brief Filename constant used by this component.
     */
    static const char *Filename; ///< Filesystem name of the persistent sequencer settings file.

    /**
     * @brief Constructs a Settings instance.
     */
    Settings();

    /**
     * @brief Returns the calibration.
     *
     * @return Reference to the calibration.
     */
    const Calibration &calibration() const { return _calibration; }
          /**
           * @brief Returns the calibration.
           *
           * @return Reference to the calibration.
           */
          Calibration &calibration()       { return _calibration; }

#ifdef CONFIG_ADVANCED_SETTINGS
    /**
     * @brief Advances d settings.
     *
     * @return Reference to the advanced settings.
     */
    const AdvancedSettings &advancedSettings() const { return _advancedSettings; }
          /**
           * @brief Advances d settings.
           *
           * @return Reference to the advanced settings.
           */
          AdvancedSettings &advancedSettings()       { return _advancedSettings; }
#endif
    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     *
     * @return `true` if read; otherwise `false`.
     */
    bool read(VersionedSerializedReader &reader);

    /**
     * @brief Writes to flash.
     */
    void writeToFlash() const;
    /**
     * @brief Reads from flash.
     *
     * @return `true` if read from flash; otherwise `false`.
     */
    bool readFromFlash();

private:
    Calibration _calibration; ///< CV-output/input calibration data.
#ifdef CONFIG_ADVANCED_SETTINGS
    AdvancedSettings _advancedSettings; ///< Advanced project/device settings.
#endif
};
