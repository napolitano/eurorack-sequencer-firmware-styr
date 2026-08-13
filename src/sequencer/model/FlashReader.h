/**
 * @file FlashReader.h
 * @brief Declares the FlashReader component used by the sequencer model.
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

#include "drivers/Flash.h"

#include <cstring>

/**
 * @brief Stores and manipulates flash reader model data.
 */
class FlashReader {
public:
    /**
     * @brief Constructs a FlashReader instance.
     *
     * @param[in] address Byte address in the target flash/storage address space.
     */
    FlashReader(uint32_t address) :
        /**
         * @brief Returns the address.
         */
        _address(reinterpret_cast<const uint8_t *>(address))
    {
    }

    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void read(void *data, size_t len) {
#ifdef PLATFORM_STM32
        std::memcpy(data, _address, len);
#endif
        _address += len;
    }

private:
    /**
     * @brief Pointer to address; `nullptr` denotes that no object/resource is assigned.
     */
    const uint8_t *_address; ///< Pointer to address; `nullptr` denotes that no object/resource is assigned.
};
