/**
 * @file InstrumentSetup.cpp
 * @brief Implements the InstrumentSetup component used by the simulator frontend.
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
#include "InstrumentSetup.h"

namespace sim {

SamplerSetup::SamplerSetup(Audio &audio) {
    std::string prefix("assets/drumkit/");

    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav", "tom2.wav" }) {
        _instruments.emplace_back(std::make_shared<DrumSampler>(audio, prefix + wav));
    }
}

MixedSetup::MixedSetup(Audio &audio) {
    std::string prefix("assets/drumkit/");

    for (const auto &wav : { "kick.wav", "snare.wav", "rim.wav", "clap.wav", "hh1.wav", "hh2.wav", "tom1.wav" }) {
        _instruments.emplace_back(std::make_shared<DrumSampler>(audio, prefix + wav));
    }

    // _instruments.emplace_back(std::make_shared<Synth>(audio));
    _instruments.emplace_back(std::make_shared<Synth>(audio));
}

} // namespace sim
