/**
 * @file Timer.h
 * @brief Declares the Timer component used by the test framework.
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

#include <cstdint>

class Timer {
public:
    Timer() {

    }

    void reset() {
        _startTime = currentTime();
    }

    uint32_t elapsed() {
        return currentTime() - _startTime;
    }


private:
    uint32_t currentTime() {
        return CURRENT_TIME();
    }

    uint32_t _startTime;
};
