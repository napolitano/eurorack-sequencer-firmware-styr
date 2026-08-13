/**
 * @file FlashLayout.h
 * @brief Defines STM32F405 flash-sector boundaries used by the bootloader updater.
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

#include <cstddef>
#include <cstdint>

namespace BootloaderFlash {

static const uint32_t SectorAddress[] = {
    0x08000000, // Sector 0, 16 Kbytes
    0x08004000, // Sector 1, 16 Kbytes
    0x08008000, // Sector 2, 16 Kbytes
    0x0800C000, // Sector 3, 16 Kbytes
    0x08010000, // Sector 4, 64 Kbytes
    0x08020000, // Sector 5, 128 Kbytes
    0x08040000, // Sector 6, 128 Kbytes
    0x08060000, // Sector 7, 128 Kbytes
    0x08080000, // Sector 8, 128 Kbytes
    0x080A0000, // Sector 9, 128 Kbytes
    0x080C0000, // Sector 10, 128 Kbytes
    0x080E0000, // Sector 11, 128 Kbytes
};

static const std::size_t SectorCount = sizeof(SectorAddress) / sizeof(SectorAddress[0]); ///< Number of STM32 flash-sector start addresses in `SectorAddress`. ///< Number of STM32 flash-sector start addresses in `SectorAddress`.

/**
 * @brief Returns the flash-sector index containing an address.
 *
 * @param[in] address Absolute address in the target flash/storage address space.
 *
 * @return Computed result in the domain described by this function.
 */
inline int sectorIndex(uint32_t address) {
    for (std::size_t i = 0; i < SectorCount; ++i) {
        if (address == SectorAddress[i]) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace BootloaderFlash
