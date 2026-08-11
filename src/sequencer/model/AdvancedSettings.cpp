/**
 * @file AdvancedSettings.cpp
 * @brief Implements the AdvancedSettings component used by the sequencer model.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include "AdvancedSettings.h"

void AdvancedSettings::clear() {
    _flags = 0;
    _language = LanguageEnglish;
}

void AdvancedSettings::write(VersionedSerializedWriter &writer) const {
    writer.write(_flags);
    writer.write(_language);
}

void AdvancedSettings::read(VersionedSerializedReader &reader) {
    clear();

    // AdvancedSettings existieren erst ab Settings::Version >= 2
    reader.read(_flags);
    reader.read(_language);

    if (_language >= LanguageCount) {
        _language = LanguageEnglish;
    }
}