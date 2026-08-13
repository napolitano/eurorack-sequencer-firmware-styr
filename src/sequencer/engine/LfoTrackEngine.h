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

/**
 * @brief Implements lfo track engine runtime processing.
 */
class LfoTrackEngine : public TrackEngine {
public:
    /**
     * @brief Constructs a LfoTrackEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] track Track model operated on by this component.
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    LfoTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine)
        /**
         * @brief Returns the track engine.
         *
         * @return The track engine value.
         */
        : TrackEngine(engine, model, track, linkedTrackEngine),
          /**
           * @brief Returns the lfo track.
           */
          _lfoTrack(track.lfoTrack())
    {
        reset();
    }

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Lfo; }

    /**
     * @brief Resets the LfoTrackEngine to its initial runtime state.
     */
    virtual void reset() override;
    /**
     * @brief Restarts the LfoTrackEngine while retaining persistent configuration.
     */
    virtual void restart() override;
    /**
     * @brief Advances the LfoTrackEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return Tick-processing result describing which outputs changed.
     */
    virtual TickResult tick(uint32_t tick) override;
    /**
     * @brief Updates the LfoTrackEngine for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    virtual void update(float dt) override;

    /**
     * @brief Reports whether the track produced visible or output activity.
     *
     * @return `true` if activity; otherwise `false`.
     */
    virtual bool activity() const override;
    /**
     * @brief Returns the current gate state for a track output channel.
     *
     * @param[in] index Zero-based gate-output channel index.
     *
     * @return `true` if gate output; otherwise `false`.
     */
    virtual bool gateOutput(int index) const override;
    /**
     * @brief Returns the current CV value for a track output channel.
     *
     * @param[in] index Zero-based CV-output channel index.
     *
     * @return Current control-voltage value for the addressed output channel.
     */
    virtual float cvOutput(int index) const override;
    /**
     * @brief Returns normalized progress through the active sequence.
     *
     * @return Normalized progress through the active sequence in the range 0..1.
     */
    virtual float sequenceProgress() const override { return _phase; }

    // Access generated random steps (read-only) so UI can preview exact values
    /**
     * @brief Returns the random steps.
     *
     * @return Reference to the random steps.
     */
    const std::vector<float> &randomSteps() const { return _randSteps; }

    // Ensure random steps are generated (callable from UI to prepare preview even when engine not running)
    /**
     * @brief Generates cached random steps when the current state requires them.
     */
    void generateRandomStepsIfNeeded();

private:
    /**
     * @brief Reference to lfo track owned by another component.
     */
    LfoTrack &_lfoTrack; ///< Reference to lfo track owned by another component.
    /**
     * @brief Runtime value representing cv value.
     */
    float _cvValue = 0.f; ///< Control-voltage value stored by the routing/engine state.
    /**
     * @brief Whether this track/component produced activity during the current processing interval.
     */
    bool _activity = false; ///< Whether this track/component produced activity during the current processing interval.
    /**
     * @brief Runtime value representing phase.
     */
    float _phase = 0.f; ///< Current normalized LFO phase used to evaluate the selected waveform.
    // Random step storage for Sync mode
    std::vector<float> _randSteps; ///< Generated random-step waveform values for the current random-step configuration.
    /**
     * @brief Most recently observed rand steps speed.
     */
    int _lastRandStepsSpeed = 0; ///< LFO speed used to generate `_randSteps`; changes invalidate the cached steps.
    /**
     * @brief Most recently observed rand waveform.
     */
    LfoTrack::Waveform _lastRandWaveform = LfoTrack::Waveform::Sine; ///< Waveform used to generate `_randSteps`; changes invalidate the cached steps.
    /**
     * @brief Most recently observed rand hi.
     */
    int16_t _lastRandHi = 0; ///< Upper random-step bound used for the cached waveform.
    /**
     * @brief Most recently observed rand low.
     */
    int16_t _lastRandLow = 0; ///< Lower random-step bound used for the cached waveform.
    // RNG for generating true-random values
    std::mt19937 _rng; ///< Pseudo-random generator used for random LFO waveforms and step values.

    // Free-mode random interpolation state
    /**
     * @brief Runtime value representing free elapsed.
     */
    float _freeElapsed = 0.f; ///< Elapsed local time accumulated by free-running LFO mode.
    /**
     * @brief Runtime value representing free interval.
     */
    float _freeInterval = 0.1f; ///< Current free-running LFO interval in engine/runtime time units.
    /**
     * @brief Runtime value representing free prev.
     */
    float _freePrev = 0.f; ///< Previous free-running sample/value.
    /**
     * @brief Runtime value representing free next.
     */
    float _freeNext = 0.f; ///< Next free-running sample/value.
    /**
     * @brief Whether free initialized is true in the current state.
     */
    bool _freeInitialized = false; ///< True after the free-running LFO has established its initial phase/timestamp state.
    // helper: regenerate random steps when parameters changed
    /**
     * @brief Ensures the cached random-step data is available and valid.
     *
     * @param[in] force Whether to bypass the normal conditional/update suppression for this operation.
     */
    void ensureRandomSteps(bool force = false);

    // Free-mode per-step tables (previous/next) for smooth morphing
    std::vector<float> _freePrevSteps; ///< Previous free-running random-step cycle used for interpolation across cycle boundaries.
    std::vector<float> _freeNextSteps; ///< Next free-running random-step cycle prepared for interpolation across cycle boundaries.
};
