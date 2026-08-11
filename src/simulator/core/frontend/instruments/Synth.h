/**
 * @file Synth.h
 * @brief Declares the Synth component used by the simulator frontend.
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

#include "../Audio.h"
#include "../Instrument.h"

#include "soloud.h"

namespace sim {

class Synth;
class Voice;

class SynthInstance : public SoLoud::AudioSourceInstance {
public:
    SynthInstance(Synth &synth);
    virtual ~SynthInstance();

    virtual unsigned int getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize) override;
    virtual bool hasEnded() override;

private:
    Synth &_synth;
    std::unique_ptr<Voice> _voice;
};

class Synth : public Instrument, SoLoud::AudioSource {
public:
    Synth(Audio &audio);
    ~Synth();

    virtual SoLoud::AudioSourceInstance *createInstance() override;

    virtual void setGate(bool gate) override;
    virtual void setCv(float cv) override;

private:
    Audio &_audio;
    int _handle;
    bool _gate = false;
    float _cv = 0.f;

    friend class SynthInstance;
};

} // namespace sim
