/**
 * @file Groove.h
 * @brief Declares the Groove component used by the sequencer engine.
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

#include "Config.h"

#include <cstdint>

namespace Groove {

static uint32_t remap(uint32_t value, uint32_t range, uint32_t newRange) {
    return (value * newRange) / range;
}

static uint32_t applySwing(uint32_t tick, int swing, uint32_t base = CONFIG_PPQN / 4) {
    uint32_t period = 2 * base;

    uint32_t beat = tick / period;
    tick %= period;

    if (tick <= base) {
        tick = remap(tick, base, (period * swing) / 100);
    } else {
        tick = period - remap(period - tick, base, (period * (100 - swing)) / 100);
    }

    return beat * period + tick;
}

} // namespace Groove
