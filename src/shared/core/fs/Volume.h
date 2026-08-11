/**
 * @file Volume.h
 * @brief Declares the Volume component used by the shared filesystem support.
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

#include "Error.h"

#include "drivers/SdCard.h"

#include <cstddef>
#include <cstdint>

namespace fs {

class Volume {
public:
    Volume(SdCard &sdcard);
    ~Volume();

    SdCard &sdcard() { return _sdcard; }

    bool available();

    Error format();

    Error mount();
    Error unmount();

    Error stats(size_t *total, size_t *free) const;
    size_t sizeTotal() const;
    size_t sizeFree() const;

private:
    SdCard &_sdcard;
    FATFS _fs;
};

} // namespace fs
