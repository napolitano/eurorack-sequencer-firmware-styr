/**
 * @file DrumSampler.cpp
 * @brief Implements the DrumSampler component used by the simulator frontend.
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
#include "DrumSampler.h"

namespace sim {

DrumSampler::DrumSampler(Audio &audio, const std::string &filename) :
    _audio(audio),
    _sample(filename)
{
}

void DrumSampler::setGate(bool gate) {
    if (gate != _gate) {
        if (gate) {
            trigger();
        }
        _gate = gate;
    }
}

void DrumSampler::setCv(float cv) {
}

void DrumSampler::trigger() {
    _audio.play(_sample);
}

} // namespace sim
