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

class FlashWriter {
public:
    FlashWriter(uint32_t address, uint32_t sector) :
        _address(address)
    {
        Flash::unlock();
        Flash::eraseSector(sector);
    }

    ~FlashWriter() {
        finish();
        Flash::lock();
    }

    void finish() {
        if (!_finished) {
            if (_pos > 0) {
                Flash::program(_address, _buffer);
            }
            _finished = true;
        }
    }

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
    uint32_t _address;
    uint32_t _buffer = 0xffffffff;
    size_t _pos = 0;
    bool _finished = false;
};
