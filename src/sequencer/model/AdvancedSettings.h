/**
 * @file AdvancedSettings.h
 * @brief Declares the AdvancedSettings component used by the sequencer model.
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
#pragma once

#include "Serialize.h"

#include <cstdint>

/**
 * @brief Stores and manipulates advanced settings model data.
 */
class AdvancedSettings {
public:
    /**
     * @brief Enumerates the supported language values.
     */
    enum Language : uint8_t {
        LanguageEnglish = 0, ///< Selects the language english language.
        LanguageGerman  = 1, ///< Selects the language german language.
        LanguageCount ///< Selects the language count language.
    };

    /**
     * @brief Enumerates the supported flag values.
     */
    enum Flag : uint32_t {
        ShowPlayCounter      = 1 << 0, ///< Selects the show play counter flag.
        EnhancedSongMode     = 1 << 1, ///< Selects the enhanced song mode flag.
        ShowHomeScreen       = 1 << 2, ///< Selects the show home screen flag.
        // weitere Flags hier anhängen
    };

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
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader);

    /**
     * @brief Returns flag.
     *
     * @param[in] flag Bit flag to query or update in the settings value.
     *
     * @return `true` if flag; otherwise `false`.
     */
    bool flag(Flag flag) const {
        return (_flags & flag) != 0;
    }

    /**
     * @brief Sets the flag.
     *
     * @param[in] flag Bit flag to query or update in the settings value.
     * @param[in] value New flag to store or apply.
     */
    void setFlag(Flag flag, bool value) {
        if (value) {
            _flags |= flag;
        } else {
            _flags &= ~flag;
        }
    }

    /**
     * @brief Returns the flags.
     *
     * @return Current advanced-settings flag bit mask.
     */
    uint32_t flags() const {
        return _flags;
    }

    /**
     * @brief Sets the flags.
     *
     * @param[in] flags Flags consumed by `setFlags()`.
     */
    void setFlags(uint32_t flags) {
        _flags = flags;
    }

    /**
     * @brief Returns the language.
     *
     * @return Configured UI language.
     */
    uint8_t language() const {
        return _language;
    }

    /**
     * @brief Sets the language.
     *
     * @param[in] language Language consumed by `setLanguage()`.
     */
    void setLanguage(uint8_t language) {
        _language = (language < LanguageCount) ? language : LanguageEnglish;
    }

private:
    uint32_t _flags; ///< Packed advanced-setting flags persisted with the settings model.
    uint8_t _language; ///< Selected UI language index; invalid values fall back to English.
};
