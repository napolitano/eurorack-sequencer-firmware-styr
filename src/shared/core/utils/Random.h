/**
 * @file Random.h
 * @brief Declares the Random component used by the shared core support.
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

class Random {
public:
    Random(uint32_t seed = 0) :
        _state(seed)
    {}

    inline uint32_t next() {
        _state = _state * 1664525L + 1013904223L;
        return _state;
    }

    float nextFloat() {
        union {
            uint32_t u;
            float f;
        } x;
        x.u = (next() >> 9) | 0x3f800000u;
        return x.f - 1.f;
    }

    inline bool nextBinary() {
        return next() < 0x80000000;
    }

    inline uint32_t nextRange(uint32_t range) {
        if (range == 0) return 0;
        return next() % range;
    }

private:
    uint32_t _state;
};
