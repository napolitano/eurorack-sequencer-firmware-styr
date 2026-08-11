/**
 * @file FnvHash.h
 * @brief Declares the FnvHash component used by the shared core support.
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
#include <cstdint>

class FnvHash {
public:
    uint32_t result() const { return _hash; }

    void operator()(uint8_t data) {
        _hash ^= data;
        _hash *= Prime;
    }

    void operator()(const void *data, size_t len) {
        const uint8_t *src = reinterpret_cast<const uint8_t *>(data);
        while (len-- > 0) {
            (*this)(*src++);
        }
    }

private:
    static constexpr uint32_t Hash = 0x811c9dc5;
    static constexpr uint32_t Prime = 0x1000193;

    uint32_t _hash = Hash;
};
