/**
 * @file VersionTag.h
 * @brief Declares the VersionTag component used by the bootloader.
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

#include "Config.h"

#include <cstdint>

/**
 * @brief Stores the firmware version marker embedded at the fixed application image offset.
 */
struct VersionTag {
    /**
     * @brief Magic value used to identify a valid encoded structure/version tag.
     */
    uint32_t magic; ///< Magic value used to identify a valid encoded structure/version tag.
    /**
     * @brief Bootloader value representing name.
     */
    char name[24]; ///< Null-terminated version or image name stored in the bootloader metadata record.
    /**
     * @brief Major component of the encoded firmware version.
     */
    uint8_t major; ///< Major component of the encoded firmware version.
    /**
     * @brief Minor component of the encoded firmware version.
     */
    uint8_t minor; ///< Minor component of the encoded firmware version.
    /**
     * @brief Revision component of the encoded firmware version.
     */
    uint16_t revision; ///< Revision component of the encoded firmware version.

    /**
     * @brief Reports whether valid.
     *
     * @return `true` if valid; otherwise `false`.
     */
    bool isValid() const {
        return magic == CONFIG_VERSION_TAG_MAGIC;
    }
};
