/**
 * @file Audio.cpp
 * @brief Implements the Audio component used by the simulator frontend.
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
#include "Audio.h"

namespace sim {

// ----------------------------------------------------------------------------
// Audio
// ----------------------------------------------------------------------------

Audio::Audio() {
    _engine.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 512);
}

Audio::~Audio() {
    _engine.deinit();
}

void Audio::play(Sample &sample) {
    _engine.play(sample._wav);
}

void Audio::stopAll() {
    _engine.stopAll();
}

// ----------------------------------------------------------------------------
// Sample
// ----------------------------------------------------------------------------

Sample::Sample(const std::string &filename) {
    _wav.load(filename.c_str());
}

} // namespace sim
