/**
 * @file TestBootloaderFlashLayout.cpp
 * @brief Verifies flash-sector mapping used while erasing application firmware.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include "Config.h"
#include "FlashLayout.h"

#include <cstdint>
#include <iostream>

namespace {
int failures = 0;
void expect(bool condition, const char *name) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << name << '\n';
    }
}
}

int main() {
    for (std::size_t i = 0; i < BootloaderFlash::SectorCount; ++i) {
        expect(BootloaderFlash::sectorIndex(BootloaderFlash::SectorAddress[i]) == static_cast<int>(i),
               "sector start maps to its hardware sector index");
    }

    expect(BootloaderFlash::sectorIndex(0x08000004) == -1, "non-boundary address is not an erase boundary");
    expect(BootloaderFlash::sectorIndex(0x0800ffff) == -1, "address before application is not a sector start");
    expect(BootloaderFlash::sectorIndex(CONFIG_APPLICATION_ADDR) == 4,
           "application starts at STM32 sector 4");
    expect(CONFIG_APPLICATION_ADDR + CONFIG_APPLICATION_SIZE == 0x08100000u,
           "application partition ends exactly at end of 1 MiB flash");

    // The updater advances from CONFIG_APPLICATION_ADDR upward. Every erase
    // boundary it can encounter must belong to sectors 4..11; sectors 0..3
    // contain the bootloader/configuration area and must never be selected.
    for (std::size_t i = 0; i < BootloaderFlash::SectorCount; ++i) {
        const uint32_t address = BootloaderFlash::SectorAddress[i];
        if (address >= CONFIG_APPLICATION_ADDR && address < CONFIG_APPLICATION_ADDR + CONFIG_APPLICATION_SIZE) {
            expect(i >= 4, "application erase boundary must not select protected sectors 0..3");
        }
    }

    if (failures != 0) {
        std::cerr << failures << " bootloader flash-layout test(s) failed\n";
        return 1;
    }
    std::cout << "Bootloader flash layout: all cases passed\n";
    return 0;
}
