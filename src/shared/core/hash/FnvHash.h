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

/**
 * @brief Maintains a 32-bit FNV-1a hash over incrementally supplied bytes.
 */
class FnvHash {
public:
    /**
     * @brief Returns the result.
     *
     * @return Current 32-bit FNV-1a hash value.
     */
    uint32_t result() const { return _hash; }

    /**
     * @brief Implements operator() for this type.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     */
    void operator()(uint8_t data) {
        _hash ^= data;
        _hash *= Prime;
    }

    /**
     * @brief Implements operator() for this type.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void operator()(const void *data, size_t len) {
        const uint8_t *src = reinterpret_cast<const uint8_t *>(data);
        while (len-- > 0) {
            (*this)(*src++);
        }
    }

private:
    /**
     * @brief Hash constant used by this component.
     */
    static constexpr uint32_t Hash = 0x811c9dc5; ///< 32-bit FNV-1a offset-basis value used to initialize the rolling hash.
    /**
     * @brief Prime constant used by this component.
     */
    static constexpr uint32_t Prime = 0x1000193; ///< 32-bit FNV-1a prime multiplied into the rolling hash after each byte.

    /**
     * @brief Current rolling hash value.
     */
    uint32_t _hash = Hash; ///< Current rolling hash value.
};
