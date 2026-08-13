/**
 * @file Slide.h
 * @brief Declares the Slide component used by the sequencer engine.
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

namespace Slide {

/**
 * @brief Applies one exponential slide/slew update toward a target value.
 *
 * @param[in] current Current value before slide processing.
 * @param[in] target Target value approached by slide processing.
 * @param[in] slideTime Configured slide-time value controlling the exponential time constant.
 * @param[in] dt Elapsed update time in seconds.
 *
 * @return Computed result in the domain described by this function.
 */
static float applySlide(float current, float target, int slideTime, float dt) {
    float tau = slideTime / 100.f;
    tau = tau * tau * 2.f;
    float coeff = tau > 0.f ? std::exp(-1.f * dt / tau) : 0.f;
    return target + coeff * (current - target);
}

} // namespace Slide
