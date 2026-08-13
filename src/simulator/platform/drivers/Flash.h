/**
 * @file Flash.h
 * @brief Declares the Flash component used by the simulator driver.
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

#include <cstdint>

/**
 * @brief Provides the flash hardware/platform abstraction.
 */
class Flash {
public:
    /**
     * @brief Re-enables normal engine updates after a lock.
     */
    static void unlock() {}
    /**
     * @brief Prevents normal engine updates during a short critical operation.
     */
    static void lock() {}
    /**
     * @brief Erases sector.
     *
     * @param[in] sector Flash/SD-card sector index.
     */
    static void eraseSector(uint32_t sector) {}
    /**
     * @brief Programs the target device or memory with the supplied data.
     *
     * @param[in] address Byte address in the target flash/storage address space.
     * @param[in] data Input data to read, decode, copy, or process.
     */
    static void program(uint32_t address, uint32_t data) {}
};
