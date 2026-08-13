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

/**
 * @brief Provides the sd card hardware/platform abstraction.
 */
class SdCard {
public:
    /**
     * @brief Constructs a SdCard instance.
     */
    SdCard() :
        /**
         * @brief Performs the data operation for this SdCard.
         *
         * @param[in] uint8_t Uint8 t supplied to the operation.
         */
        _data(new uint8_t[SectorCount * SectorSize])
    {
        std::ifstream ifs("sdcard.iso");
        ifs.read(reinterpret_cast<char *>(_data.get()), SectorCount * SectorSize);
    }

    /**
     * @brief Initializes the SdCard and its runtime resources.
     */
    void init() {
    }

    /**
     * @brief Returns the available.
     *
     * @return `true` if available; otherwise `false`.
     */
    bool available() {
        return true;
    }

    /**
     * @brief Writes protected.
     *
     * @return `true` if write protected; otherwise `false`.
     */
    bool writeProtected() {
        return false;
    }

    /**
     * @brief Returns the sector count.
     *
     * @return Number of addressable sectors on the simulated or physical SD card.
     */
    size_t sectorCount() const { return SectorCount; }
    /**
     * @brief Returns the sector size.
     *
     * @return Size of one SD-card sector in bytes.
     */
    size_t sectorSize() const { return SectorSize; }

    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] sector Flash/SD-card sector index.
     * @param[in] count Number of items covered by the operation.
     *
     * @return `true` if read; otherwise `false`.
     */
    bool read(uint8_t *buf, uint32_t sector, uint8_t count) {
        ASSERT(sector >= 0 && sector + count <= SectorCount, "invalid read range");
        memcpy(buf, &_data[sector * SectorSize], count * SectorSize);
        return true;
    }

    /**
     * @brief Writes data to the underlying destination.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] sector Flash/SD-card sector index.
     * @param[in] count Number of items covered by the operation.
     *
     * @return `true` if write; otherwise `false`.
     */
    bool write(const uint8_t *buf, uint32_t sector, uint8_t count) {
        ASSERT(sector >= 0 && sector + count <= SectorCount, "invalid write range");
        memcpy(&_data[sector * SectorSize], buf, count * SectorSize);
        return true;
    }

    /**
     * @brief Synchronizes the SdCard with its current source of truth.
     */
    void sync() {
        std::ofstream ofs("sdcard.iso");
        ofs.write(reinterpret_cast<const char *>(_data.get()), SectorCount * SectorSize);
        ofs.close();
    }

private:
    /**
     * @brief Number of sector entries supported by the implementation.
     */
    static constexpr size_t SectorCount = 1024; ///< Number of sector entries supported by the implementation.
    /**
     * @brief Sector size constant used by this component.
     */
    static constexpr size_t SectorSize = 512; ///< SD-card logical sector size, in bytes.

    std::unique_ptr<uint8_t[]> _data; ///< Owned byte buffer backing the simulated SD-card image.
};
