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

/**
 * @brief Provides audio behavior for the desktop simulator.
 */
class Audio {
public:
    /**
     * @brief Constructs a Audio instance.
     */
    Audio();
    /**
     * @brief Destroys the Audio instance.
     */
    ~Audio();

    /**
     * @brief Returns the engine.
     *
     * @return Reference to the engine.
     */
    SoLoud::Soloud &engine() { return _engine; }

    /**
     * @brief Starts playback of the configured sound/sample.
     *
     * @param[in] sample Audio/CV/data sample consumed by the operation.
     */
    void play(Sample &sample);
    /**
     * @brief Stops all.
     */
    void stopAll();

private:
    SoLoud::Soloud _engine; ///< SoLoud engine instance that owns the simulator audio backend.
};

/**
 * @brief Provides sample behavior for the desktop simulator.
 */
class Sample {
public:
    typedef std::shared_ptr<Sample> Ptr;

    /**
     * @brief Constructs a Sample instance.
     *
     * @param[in] filename File name/path used for the operation.
     */
    Sample(const std::string &filename);

private:
    SoLoud::Wav _wav; ///< Decoded waveform sample played by this simulator audio source.

    friend class Audio;
};

} // namespace sim
