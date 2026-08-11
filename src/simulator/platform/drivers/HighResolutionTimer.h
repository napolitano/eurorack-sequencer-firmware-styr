/**
 * @file HighResolutionTimer.h
 * @brief Declares the HighResolutionTimer component used by the simulator driver.
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

#include <chrono>

#include <cstdint>

namespace detail {
    static std::chrono::time_point<std::chrono::high_resolution_clock> start;
}

class HighResolutionTimer {
public:
    static void init() {
        detail::start = std::chrono::high_resolution_clock::now();
    }

    static uint32_t us() {
        auto current = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(current - detail::start)).count();
    }

};
