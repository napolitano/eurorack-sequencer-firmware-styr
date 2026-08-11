/**
 * @file Settings.cpp
 * @brief Implements the Settings component used by the sequencer model.
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
#include "Settings.h"
#include "FlashWriter.h"
#include "FlashReader.h"

const char *Settings::Filename = "SETTINGS.DAT";

Settings::Settings() {
    clear();
}

void Settings::clear() {
    _calibration.clear();
#ifdef CONFIG_ADVANCED_SETTINGS
    _advancedSettings.clear();
#endif
}

void Settings::write(VersionedSerializedWriter &writer) const {
    _calibration.write(writer);
#ifdef CONFIG_ADVANCED_SETTINGS
    _advancedSettings.write(writer);
#endif
    writer.writeHash();
}

bool Settings::read(VersionedSerializedReader &reader) {
    clear();

    _calibration.read(reader);
#ifdef CONFIG_ADVANCED_SETTINGS
    _advancedSettings.read(reader);
#endif

    bool success = reader.checkHash();
    if (!success) {
        clear();
    }

    return success;
}

void Settings::writeToFlash() const {
    FlashWriter flashWriter(CONFIG_SETTINGS_FLASH_ADDR, CONFIG_SETTINGS_FLASH_SECTOR);

    VersionedSerializedWriter writer(
        [&flashWriter] (const void *data, size_t len) { flashWriter.write(data, len); },
        Version
    );

    write(writer);

    flashWriter.finish();
}

bool Settings::readFromFlash() {
    FlashReader flashReader(CONFIG_SETTINGS_FLASH_ADDR);

    VersionedSerializedReader reader(
        [&flashReader] (void *data, size_t len) { flashReader.read(data, len); },
        Version
    );

    return read(reader);
}
