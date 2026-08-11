/**
 * @file UpdateFile.h
 * @brief Declares the UpdateFile component used by the bootloader.
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

#include "VersionTag.h"

#include <cstdlib>

class UpdateFile {
public:
    static bool open(VersionTag &version, size_t &size, uint8_t md5[16], char *errorStr, size_t errorLen);
    static bool rewind(char *errorStr, size_t errorLen);
    static bool read(void *readBuf, size_t readLen, char *errorStr, size_t errorLen);
};
