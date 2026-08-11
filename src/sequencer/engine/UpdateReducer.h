/**
 * @file UpdateReducer.h
 * @brief Declares the UpdateReducer component used by the sequencer engine.
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

#include "os/os.h"

#include <cstdint>

template<uint32_t Interval>
class UpdateReducer {
public:
    bool update() {
        uint32_t currentTick = os::ticks();
        if (currentTick >= _lastUpdate + Interval) {
            _lastUpdate = currentTick;
            return true;
        }
        return false;
    }

private:
    uint32_t _lastUpdate = 0;
};
