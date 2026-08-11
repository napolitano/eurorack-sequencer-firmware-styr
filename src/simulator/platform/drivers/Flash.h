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

class Flash {
public:
    static void unlock() {}
    static void lock() {}
    static void eraseSector(uint32_t sector) {}
    static void program(uint32_t address, uint32_t data) {}
};
