/**
 * @file SdCard.h
 * @brief Declares the SdCard component used by the simulator driver.
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

#include "core/Debug.h"

#include <memory>
#include <fstream>

#include <cstring>
#include <cstddef>
#include <cstdint>

class SdCard {
public:
    SdCard() :
        _data(new uint8_t[SectorCount * SectorSize])
    {
        std::ifstream ifs("sdcard.iso");
        ifs.read(reinterpret_cast<char *>(_data.get()), SectorCount * SectorSize);
    }

    void init() {
    }

    bool available() {
        return true;
    }

    bool writeProtected() {
        return false;
    }

    size_t sectorCount() const { return SectorCount; }
    size_t sectorSize() const { return SectorSize; }

    bool read(uint8_t *buf, uint32_t sector, uint8_t count) {
        ASSERT(sector >= 0 && sector + count <= SectorCount, "invalid read range");
        memcpy(buf, &_data[sector * SectorSize], count * SectorSize);
        return true;
    }

    bool write(const uint8_t *buf, uint32_t sector, uint8_t count) {
        ASSERT(sector >= 0 && sector + count <= SectorCount, "invalid write range");
        memcpy(&_data[sector * SectorSize], buf, count * SectorSize);
        return true;
    }

    void sync() {
        std::ofstream ofs("sdcard.iso");
        ofs.write(reinterpret_cast<const char *>(_data.get()), SectorCount * SectorSize);
        ofs.close();
    }

private:
    static constexpr size_t SectorCount = 1024;
    static constexpr size_t SectorSize = 512;

    std::unique_ptr<uint8_t[]> _data;
};
