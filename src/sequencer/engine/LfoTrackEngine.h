/**
 * @file LfoTrackEngine.h
 * @brief Declares the LfoTrackEngine component used by the sequencer engine.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "TrackEngine.h"
#include "model/Track.h"
#include "model/LfoTrack.h"
#include <vector>
#include <random>
#include <cstdint>

class LfoTrackEngine : public TrackEngine {
public:
    LfoTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine)
        : TrackEngine(engine, model, track, linkedTrackEngine),
          _lfoTrack(track.lfoTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Lfo; }

    virtual void reset() override;
    virtual void restart() override;
    virtual TickResult tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual bool activity() const override;
    virtual bool gateOutput(int index) const override;
    virtual float cvOutput(int index) const override;
    virtual float sequenceProgress() const override { return _phase; }

    // Access generated random steps (read-only) so UI can preview exact values
    const std::vector<float> &randomSteps() const { return _randSteps; }

    // Ensure random steps are generated (callable from UI to prepare preview even when engine not running)
    void generateRandomStepsIfNeeded();

private:
    LfoTrack &_lfoTrack;
    float _cvValue = 0.f;
    bool _activity = false;
    float _phase = 0.f;
    // Random step storage for Sync mode
    std::vector<float> _randSteps;
    int _lastRandStepsSpeed = 0;
    LfoTrack::Waveform _lastRandWaveform = LfoTrack::Waveform::Sine;
    int16_t _lastRandHi = 0;
    int16_t _lastRandLow = 0;
    // RNG for generating true-random values
    std::mt19937 _rng;

    // Free-mode random interpolation state
    float _freeElapsed = 0.f;
    float _freeInterval = 0.1f;
    float _freePrev = 0.f;   // legacy single-value kept for compatibility
    float _freeNext = 0.f;
    bool _freeInitialized = false;
    // helper: regenerate random steps when parameters changed
    void ensureRandomSteps(bool force = false);

    // Free-mode per-step tables (previous/next) for smooth morphing
    std::vector<float> _freePrevSteps;
    std::vector<float> _freeNextSteps;
};
