/**
 * @file Audio.h
 * @brief Declares the Audio component used by the simulator frontend.
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

#include "Common.h"

#include "soloud.h"
#include "soloud_wav.h"

namespace sim {

class Sample;

class Audio {
public:
    Audio();
    ~Audio();

    SoLoud::Soloud &engine() { return _engine; }

    void play(Sample &sample);
    void stopAll();

private:
    SoLoud::Soloud _engine;
};

class Sample {
public:
    typedef std::shared_ptr<Sample> Ptr;

    Sample(const std::string &filename);

private:
    SoLoud::Wav _wav;

    friend class Audio;
};

} // namespace sim
