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

struct VersionTag {
    uint32_t magic;
    char name[24];
    uint8_t major;
    uint8_t minor;
    uint16_t revision;

    bool isValid() const {
        return magic == CONFIG_VERSION_TAG_MAGIC;
    }
};
