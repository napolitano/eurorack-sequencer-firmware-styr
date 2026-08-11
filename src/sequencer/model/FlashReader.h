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

class FlashReader {
public:
    FlashReader(uint32_t address) :
        _address(reinterpret_cast<const uint8_t *>(address))
    {
    }

    void read(void *data, size_t len) {
#ifdef PLATFORM_STM32
        std::memcpy(data, _address, len);
#endif
        _address += len;
    }

private:
    const uint8_t *_address;
};
