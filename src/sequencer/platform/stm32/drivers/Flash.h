/**
 * @file Flash.h
 * @brief Declares the Flash component used by the STM32 sequencer driver.
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

#include "SystemConfig.h"

#include <libopencm3/stm32/flash.h>

#include <cstdint>

class Flash {
public:
    static void unlock() {
        flash_unlock();
    }

    static void lock() {
        flash_lock();
    }

    static void eraseSector(uint32_t sector) {
        flash_erase_sector(sector, 2);
        flash_wait_for_last_operation();
    }

    static void program(uint32_t address, uint32_t data) {
        flash_program_word(address, data);
        flash_wait_for_last_operation();
    }
};
