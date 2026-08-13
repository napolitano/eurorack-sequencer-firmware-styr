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

/**
 * @brief Limits expensive updates to a configured cadence while preserving elapsed time.
 */
template<uint32_t Interval>
/**
 * @brief Coalesces frequent update requests so work executes no more often than the configured interval.
 */
class UpdateReducer {
public:
    /**
     * @brief Updates the UpdateReducer for the current service cycle.
     *
     * @return `true` if update; otherwise `false`.
     */
    bool update() {
        uint32_t currentTick = os::ticks();
        if (currentTick >= _lastUpdate + Interval) {
            _lastUpdate = currentTick;
            return true;
        }
        return false;
    }

private:
    /**
     * @brief Most recently observed update.
     */
    uint32_t _lastUpdate = 0; ///< System tick of the most recent accepted update, used to enforce the reduction interval.
};
