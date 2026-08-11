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

class Settings {
public:
#ifdef CONFIG_ADVANCED_SETTINGS
    static constexpr uint32_t Version = 2;
#else
    static constexpr uint32_t Version = 1;
#endif

    static const char *Filename;

    Settings();

    const Calibration &calibration() const { return _calibration; }
          Calibration &calibration()       { return _calibration; }

#ifdef CONFIG_ADVANCED_SETTINGS
    const AdvancedSettings &advancedSettings() const { return _advancedSettings; }
          AdvancedSettings &advancedSettings()       { return _advancedSettings; }
#endif
    void clear();

    void write(VersionedSerializedWriter &writer) const;
    bool read(VersionedSerializedReader &reader);

    void writeToFlash() const;
    bool readFromFlash();

private:
    Calibration _calibration;
#ifdef CONFIG_ADVANCED_SETTINGS
    AdvancedSettings _advancedSettings;
#endif
};
