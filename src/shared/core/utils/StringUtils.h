/**
 * @file StringUtils.h
 * @brief Declares the StringUtils component used by the shared core support.
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

#include <cstdlib>

/**
 * @brief Provides bounded string-copy and formatting helpers used by the firmware.
 */
class StringUtils {
public:

    /**
     * @brief Copies the supplied value/data into the destination.
     *
     * @param[in] dst Destination object/buffer/value.
     * @param[in] src Source object/buffer/value.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    static void copy(char *dst, const char *src, size_t len) {
        if (len == 0) {
            return;
        }
        while (len-- > 1 && *src != '\0') {
            *dst++ = *src++;
        }
        *dst = '\0';
    }

};
