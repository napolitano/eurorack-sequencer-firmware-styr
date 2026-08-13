/**
 * @file SequenceUtils.h
 * @brief Declares the SequenceUtils component used by the sequencer engine.
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

#include "core/Debug.h"

namespace SequenceUtils {

/**
 * @brief Rotates a step index within an inclusive sequence range with wrap-around.
 *
 * @param[in] step Edit step size or zero-based sequence step as defined by the operation.
 * @param[in] firstStep First inclusive active sequence-step index.
 * @param[in] lastStep Last inclusive active sequence-step index.
 * @param[in] rotate Signed number of steps by which to rotate the position.
 *
 * @return Computed result in the domain described by this function.
 */
static int rotateStep(int step, int firstStep, int lastStep, int rotate) {
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    int stepCount = lastStep - firstStep + 1;
    step = firstStep + (step - firstStep + rotate) % stepCount;
    return step + (step < 0 ? stepCount : 0);
}

} // namespace SequenceUtils
