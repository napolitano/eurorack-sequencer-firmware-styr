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

/**
 * @brief Validates and streams a firmware update image from the SD card.
 */
class UpdateFile {
public:
    /**
     * @brief Opens the requested page, file, dialog, or resource.
     *
     * @param[in] version Firmware or serialized-format version associated with the operation.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] md5 Expected or computed 16-byte MD5 digest.
     * @param[in] errorStr Buffer or text receiving a human-readable error description.
     * @param[in] errorLen Capacity/length of the error string buffer.
     *
     * @return `true` if open; otherwise `false`.
     */
    static bool open(VersionTag &version, size_t &size, uint8_t md5[16], char *errorStr, size_t errorLen);
    /**
     * @brief Rewinds the underlying stream/file to its initial position.
     *
     * @param[in] errorStr Buffer or text receiving a human-readable error description.
     * @param[in] errorLen Capacity/length of the error string buffer.
     *
     * @return `true` if rewind; otherwise `false`.
     */
    static bool rewind(char *errorStr, size_t errorLen);
    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] readBuf Destination buffer that receives bytes read from the update image.
     * @param[in] readLen Number of bytes requested from or returned by the update-image read.
     * @param[in] errorStr Buffer or text receiving a human-readable error description.
     * @param[in] errorLen Capacity/length of the error string buffer.
     *
     * @return `true` if read; otherwise `false`.
     */
    static bool read(void *readBuf, size_t readLen, char *errorStr, size_t errorLen);
};
