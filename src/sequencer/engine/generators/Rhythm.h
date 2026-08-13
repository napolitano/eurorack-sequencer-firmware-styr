/**
 * @file Rhythm.h
 * @brief Declares the Rhythm component used by the sequencer generator.
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

#include "RhythmString.h"

namespace Rhythm {

    typedef RhythmString<CONFIG_STEP_COUNT> Pattern;

    /**
     * @brief Generates a Euclidean rhythm pattern for the requested beats and steps.
     *
     * @param[in] beats Number of active pulses distributed across the Euclidean pattern.
     * @param[in] steps Fixed-size sequence-step array to modify.
     *
     * @return Generated rhythm pattern containing the requested pulse distribution.
     */
    Pattern euclidean(int beats, int steps);

} // namespace Rhythm
