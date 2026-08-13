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

/**
 * @brief Implements the project pseudo-random number generator and range helpers.
 */
class Random {
public:
    /**
     * @brief Constructs a Random instance.
     *
     * @param[in] seed Seed used to initialize the pseudo-random number generator.
     */
    Random(uint32_t seed = 0) :
        /**
         * @brief Returns the state.
         */
        _state(seed)
    {}

    /**
     * @brief Returns the next.
     *
     * @return Next pseudo-random integer produced by the generator.
     */
    inline uint32_t next() {
        _state = _state * 1664525L + 1013904223L;
        return _state;
    }

    /**
     * @brief Returns the next float.
     *
     * @return Next pseudo-random floating-point value in the generator-defined unit interval.
     */
    float nextFloat() {
        union {
            uint32_t u;
            float f;
        } x;
        x.u = (next() >> 9) | 0x3f800000u;
        return x.f - 1.f;
    }

    /**
     * @brief Returns the next binary.
     *
     * @return `true` if next binary; otherwise `false`.
     */
    inline bool nextBinary() {
        return next() < 0x80000000;
    }

    /**
     * @brief Returns next range.
     *
     * @param[in] range Inclusive/exclusive value span as defined by the owning API.
     *
     * @return Next generated/random range value.
     */
    inline uint32_t nextRange(uint32_t range) {
        if (range == 0) return 0;
        return next() % range;
    }

private:
    /**
     * @brief Runtime value representing state.
     */
    uint32_t _state; ///< Internal state word used by the algorithm or state machine.
};
