/**
 * @file TestBootloaderFlashLayout.cpp
 * @brief Unity tests for flash-sector mapping used while erasing application firmware.
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
#include <unity.h>

#include "../../../src/bootloader/Config.h"
#include "FlashLayout.h"

#include <cstddef>
#include <cstdint>

void setUp() {}
void tearDown() {}

void test_every_sector_start_maps_to_hardware_sector_index() {
    for (std::size_t i = 0; i < BootloaderFlash::SectorCount; ++i) {
        TEST_ASSERT_EQUAL_INT(
            static_cast<int>(i),
            BootloaderFlash::sectorIndex(BootloaderFlash::SectorAddress[i]));
    }
}

void test_sector_addresses_are_strictly_increasing_and_cover_stm32f405_flash() {
    TEST_ASSERT_EQUAL_size_t(12, BootloaderFlash::SectorCount);
    TEST_ASSERT_EQUAL_HEX32(0x08000000u, BootloaderFlash::SectorAddress[0]);
    TEST_ASSERT_EQUAL_HEX32(0x080e0000u, BootloaderFlash::SectorAddress[11]);

    for (std::size_t i = 1; i < BootloaderFlash::SectorCount; ++i) {
        TEST_ASSERT_GREATER_THAN_UINT32(
            BootloaderFlash::SectorAddress[i - 1],
            BootloaderFlash::SectorAddress[i]);
    }
}

void test_addresses_inside_sectors_are_not_mistaken_for_erase_boundaries() {
    for (std::size_t i = 0; i < BootloaderFlash::SectorCount; ++i) {
        TEST_ASSERT_EQUAL_INT(-1, BootloaderFlash::sectorIndex(BootloaderFlash::SectorAddress[i] + 4u));
    }
}

void test_non_boundaries_and_application_partition_edges_are_correct() {
    TEST_ASSERT_EQUAL_INT(-1, BootloaderFlash::sectorIndex(0x08000004));
    TEST_ASSERT_EQUAL_INT(-1, BootloaderFlash::sectorIndex(0x0800ffff));
    TEST_ASSERT_EQUAL_INT(4, BootloaderFlash::sectorIndex(CONFIG_APPLICATION_ADDR));
    TEST_ASSERT_EQUAL_HEX32(
        0x08100000u,
        CONFIG_APPLICATION_ADDR + CONFIG_APPLICATION_SIZE);
}

void test_application_erase_boundaries_never_select_protected_sectors() {
    for (std::size_t i = 0; i < BootloaderFlash::SectorCount; ++i) {
        const uint32_t address = BootloaderFlash::SectorAddress[i];
        if (address >= CONFIG_APPLICATION_ADDR &&
            address < CONFIG_APPLICATION_ADDR + CONFIG_APPLICATION_SIZE) {
            TEST_ASSERT_GREATER_OR_EQUAL_UINT32(4u, static_cast<uint32_t>(i));
        }
    }
}

void test_protected_bootloader_and_hwconfig_sectors_are_outside_application_partition() {
    for (std::size_t i = 0; i < 4; ++i) {
        TEST_ASSERT_LESS_THAN_UINT32(CONFIG_APPLICATION_ADDR, BootloaderFlash::SectorAddress[i]);
    }
    for (std::size_t i = 4; i < BootloaderFlash::SectorCount; ++i) {
        TEST_ASSERT_GREATER_OR_EQUAL_UINT32(CONFIG_APPLICATION_ADDR, BootloaderFlash::SectorAddress[i]);
        TEST_ASSERT_LESS_THAN_UINT32(
            CONFIG_APPLICATION_ADDR + CONFIG_APPLICATION_SIZE,
            BootloaderFlash::SectorAddress[i]);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_every_sector_start_maps_to_hardware_sector_index);
    RUN_TEST(test_sector_addresses_are_strictly_increasing_and_cover_stm32f405_flash);
    RUN_TEST(test_addresses_inside_sectors_are_not_mistaken_for_erase_boundaries);
    RUN_TEST(test_non_boundaries_and_application_partition_edges_are_correct);
    RUN_TEST(test_application_erase_boundaries_never_select_protected_sectors);
    RUN_TEST(test_protected_bootloader_and_hwconfig_sectors_are_outside_application_partition);
    return UNITY_END();
}
