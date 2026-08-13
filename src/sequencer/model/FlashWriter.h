/**
 * @file FlashWriter.h
 * @brief Declares the FlashWriter component used by the sequencer model.
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
 * @brief Stores and manipulates flash writer model data.
 */
class FlashWriter {
public:
    /**
     * @brief Constructs a FlashWriter instance.
     *
     * @param[in] address Byte address in the target flash/storage address space.
     * @param[in] sector Flash/SD-card sector index.
     */
    FlashWriter(uint32_t address, uint32_t sector) :
        /**
         * @brief Returns the address.
         */
        _address(address)
    {
        Flash::unlock();
        Flash::eraseSector(sector);
    }

    /**
     * @brief Destroys the FlashWriter instance.
     */
    ~FlashWriter() {
        finish();
        Flash::lock();
    }

    /**
     * @brief Finalizes the current operation and commits pending result state.
     */
    void finish() {
        if (!_finished) {
            if (_pos > 0) {
                Flash::program(_address, _buffer);
            }
            _finished = true;
        }
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void write(const void *data, size_t len) {
        const uint8_t *src = static_cast<const uint8_t *>(data);
        uint8_t *buffer = reinterpret_cast<uint8_t *>(&_buffer);

        while (len > 0) {
            size_t chunk = std::min(len, sizeof(_buffer) - _pos);
            std::memcpy(&buffer[_pos], src, chunk);
            _pos += chunk;
            src += chunk;
            len -= chunk;
            if (_pos == sizeof(_buffer)) {
                Flash::program(_address, _buffer);
                _pos = 0;
                _buffer = 0xffffffff;
                _address += sizeof(_buffer);
            }
        }
    }

private:
    uint32_t _address; ///< Current byte address in the target storage/flash address space.
    /**
     * @brief Backing buffer used to stage data for the surrounding operation.
     */
    uint32_t _buffer = 0xffffffff; ///< Backing buffer used to stage data for the surrounding operation.
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    size_t _pos = 0; ///< Current read/write position within the active buffer/stream.
    /**
     * @brief Whether finished is true in the current state.
     */
    bool _finished = false; ///< True after the operation has reached its terminal state and no additional payload remains to be processed.
};
