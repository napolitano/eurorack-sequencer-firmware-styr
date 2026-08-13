/**
 * @file CurveTrackEngine.h
 * @brief Declares the CurveTrackEngine component used by the sequencer engine.
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

#include "TrackEngine.h"
#include "SequenceState.h"
#include "SortedQueue.h"
#include "CurveRecorder.h"

#include "model/Track.h"

/**
 * @brief Evaluates Curve sequences, gates and recording state in either clock-aligned or free-running timing.
 */
class CurveTrackEngine : public TrackEngine {
public:
    /**
     * @brief Constructs a CurveTrackEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] track Track model operated on by this component.
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    CurveTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        /**
         * @brief Returns the track engine.
         */
        TrackEngine(engine, model, track, linkedTrackEngine),
        /**
         * @brief Returns the curve track.
         */
        _curveTrack(track.curveTrack())
    {
        reset();
    }

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Curve; }

    /**
     * @brief Resets the CurveTrackEngine to its initial runtime state.
     */
    virtual void reset() override;
    /**
     * @brief Restarts the CurveTrackEngine while retaining persistent configuration.
     */
    virtual void restart() override;
    /**
     * @brief Advances the CurveTrackEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return Tick-processing result describing which outputs changed.
     */
    virtual TickResult tick(uint32_t tick) override;
    /**
     * @brief Updates the CurveTrackEngine for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    virtual void update(float dt) override;

    /**
     * @brief Applies the pending pattern change to this track engine.
     */
    virtual void changePattern() override;

    /**
     * @brief Returns timing/state data exposed to linked tracks.
     *
     * @return Pointer to the link data; `nullptr` when no value is available.
     */
    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    /**
     * @brief Reports whether the track produced visible or output activity.
     *
     * @return `true` if activity; otherwise `false`.
     */
    virtual bool activity() const override { return _activity; }
    /**
     * @brief Returns the current gate state for a track output channel.
     *
     * @param[in] index Zero-based gate-output channel index.
     *
     * @return `true` if gate output; otherwise `false`.
     */
    virtual bool gateOutput(int index) const override { return _gateOutput; }
    /**
     * @brief Returns the current CV value for a track output channel.
     *
     * @param[in] index Zero-based CV-output channel index.
     *
     * @return Current control-voltage value for the addressed output channel.
     */
    virtual float cvOutput(int index) const override { return _cvOutput; }
    /**
     * @brief Returns the normalized position within the active sequence.
     * @return A value in the range 0..1; a single-step sequence reports 0.
     */
    virtual float sequenceProgress() const override {
        if (_currentStep < 0) {
            return 0.f;
        }
        const int span = _sequence->lastStep() - _sequence->firstStep();
        return span > 0 ? float(_currentStep - _sequence->firstStep()) / span : 0.f;
    }

    /**
     * @brief Returns the sequence.
     *
     * @return Reference to the sequence.
     */
    const CurveSequence &sequence() const { return *_sequence; }
    /**
     * @brief Reports whether active sequence.
     *
     * @param[in] sequence Sequence to inspect or modify.
     *
     * @return `true` if active sequence; otherwise `false`.
     */
    bool isActiveSequence(const CurveSequence &sequence) const { return &sequence == _sequence; }

    /**
     * @brief Returns the current step.
     *
     * @return Zero-based index of the current sequence step, or the documented sentinel before a step is active.
     */
    int currentStep() const { return _currentStep; }
    /**
     * @brief Returns the current step fraction.
     *
     * @return Fractional position within the active curve step.
     */
    float currentStepFraction() const { return _currentStepFraction; }

    /**
     * @brief Enumerates the supported monitor level values.
     */
    enum class MonitorLevel { Min, Max };

    /**
     * @brief Sets the monitor step.
     *
     * @param[in] index Zero-based monitor step index.
     */
    void setMonitorStep(int index) { _monitorStepIndex = (index >= 0 && index < CONFIG_STEP_COUNT) ? index : -1; }
    /**
     * @brief Sets the monitor step level.
     *
     * @param[in] level Level or range selection to apply.
     */
    void setMonitorStepLevel(MonitorLevel level) { _monitorStepLevel = level; }

private:
    /**
     * @brief Schedules events for the selected sequence step.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void triggerStep(uint32_t tick, uint32_t divisor);
    /**
     * @brief Updates the continuous curve output within the current step.
     *
     * @param[in] relativeTick Tick offset relative to the current sequence or step origin.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void updateOutput(uint32_t relativeTick, uint32_t divisor);

    /**
     * @brief Reports whether curve recording is currently active.
     *
     * @return `true` if recording; otherwise `false`.
     */
    bool isRecording() const;
    /**
     * @brief Refreshes the value sampled by curve recording.
     */
    void updateRecordValue();
    /**
     * @brief Updates curve recording for the current relative tick.
     *
     * @param[in] relativeTick Tick offset relative to the current sequence or step origin.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void updateRecording(uint32_t relativeTick, uint32_t divisor);

    /**
     * @brief Persistent curve-track model operated on by this engine.
     */
    CurveTrack &_curveTrack; ///< Persistent curve-track model operated on by this engine.

    /**
     * @brief Timing and traversal metadata exposed to linked tracks.
     */
    TrackLinkData _linkData; ///< Timing and traversal metadata exposed to linked tracks.

    /**
     * @brief Runtime value representing record value.
     */
    float _recordValue; ///< Most recently sampled input value used by curve recording.
    /**
     * @brief Recorder state used by the current track engine.
     */
    CurveRecorder _recorder; ///< Recorder state used by the current track engine.

    /**
     * @brief Zero-based monitor step index; a negative/sentinel value represents no selection where applicable.
     */
    int _monitorStepIndex = -1; ///< Zero-based monitor step index; a negative/sentinel value represents no selection where applicable.
    MonitorLevel _monitorStepLevel = MonitorLevel::Min; ///< Curve monitor level currently published for the monitored step.

    /**
     * @brief Currently active sequence.
     */
    CurveSequence *_sequence; ///< Currently active sequence.
    /**
     * @brief Sequence used when fill mode selects alternate pattern data.
     */
    CurveSequence *_fillSequence; ///< Sequence used when fill mode selects alternate pattern data.
    /**
     * @brief Tick position used by free-running mode relative to its local phase origin.
     */
    uint32_t _freeRelativeTick = 0; ///< Tick position used by free-running mode relative to its local phase origin.
    /**
     * @brief Traversal state for the active sequence.
     */
    SequenceState _sequenceState; ///< Traversal state for the active sequence.
    /**
     * @brief Zero-based index of the currently active sequence step.
     */
    int _currentStep; ///< Zero-based index of the currently active sequence step.
    /**
     * @brief Normalized position within the current curve step.
     */
    float _currentStepFraction; ///< Normalized phase within the active curve step, in the range 0..1.
    /**
     * @brief Whether shape variation is true in the current state.
     */
    bool _shapeVariation; ///< Whether the current curve step selected its probabilistic shape variation.
    CurveTrack::FillMode _fillMode; ///< Active fill mode controlling the behavior of `CurveTrackEngine`.

    /**
     * @brief Whether this track/component produced activity during the current processing interval.
     */
    bool _activity; ///< Whether this track/component produced activity during the current processing interval.
    /**
     * @brief Current digital gate output state.
     */
    bool _gateOutput; ///< Current digital gate output state.
    /**
     * @brief Current control-voltage output value in the engine output domain.
     */
    float _cvOutput = 0.f; ///< Current control-voltage output value in the engine output domain.
    /**
     * @brief Target control-voltage value used by slide/interpolation processing.
     */
    float _cvOutputTarget = 0.f; ///< Target control-voltage value used by slide/interpolation processing.

    /**
     * @brief Describes a scheduled gate transition and its timing metadata.
     */
    struct Gate {
        /**
         * @brief Current absolute sequencer engine tick.
         */
        uint32_t tick; ///< Current absolute sequencer engine tick.
        /**
         * @brief Whether gate is true in the current state.
         */
        bool gate; ///< True when the generated/current step requests an active gate.
    };

    /**
     * @brief Defines the ordering relation used by the enclosing sorted event queue.
     */
    struct GateCompare {
        /**
         * @brief Compares two values according to this type's ordering or equality semantics.
         *
         * @param[in] a Left/first value used by the comparison or binary operation.
         * @param[in] b Right/second value used by the comparison or binary operation.
         *
         * @return `true` if operator(); otherwise `false`.
         */
        bool operator()(const Gate &a, const Gate &b) {
            return a.tick < b.tick;
        }
    };

    /**
     * @brief Chronologically ordered pending gate events.
     */
    SortedQueue<Gate, 16, GateCompare> _gateQueue; ///< Chronologically ordered pending gate events.
};
