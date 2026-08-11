/**
 * @file Math.cpp
 * @brief Implements the Math component used by the shared math support.
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
#include "Math.h"

uint32_t nextPowerOfTwo(uint32_t value, bool strict) {
    if (value == 0) {
        return 1;
    }
    if (!strict) {
        --value;
    }
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value + 1;
}

uint32_t prevPowerOfTwo(uint32_t value, bool strict) {
    if (value == 0) {
        return 0;
    }
    if (strict) {
        --value;
    }
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    return value - (value >> 1);
}
