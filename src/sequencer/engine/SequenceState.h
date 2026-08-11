/**
 * @file SequenceState.h
 * @brief Declares the SequenceState component used by the sequencer engine.
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

#include "model/Types.h"

#include "core/utils/Random.h"

#include <cstdint>

class SequenceState {
public:
    int step() const { return _step; }
    int prevStep() const { return _prevStep; }
    int direction() const { return _direction; }
    uint32_t iteration() const { return _iteration; }

    void reset();

    void advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng);
    void advanceAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng);

private:
    void advanceRandomWalk(int firstStep, int lastStep, Random &rng);

    int8_t _step;
    int8_t _prevStep;
    int8_t _direction;
    uint32_t _iteration;
};
