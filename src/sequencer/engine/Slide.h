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

static float applySlide(float current, float target, int slideTime, float dt) {
    float tau = slideTime / 100.f;
    tau = tau * tau * 2.f;
    float coeff = tau > 0.f ? std::exp(-1.f * dt / tau) : 0.f;
    return target + coeff * (current - target);
}

} // namespace Slide
