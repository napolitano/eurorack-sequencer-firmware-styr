/**
 * @file NoteTrackEngine.h
 * @brief Declares the NoteTrackEngine component used by the sequencer engine.
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
#include "Groove.h"
#include "RecordHistory.h"
#include "StepRecorder.h"

/**
 * @brief Implements note track engine runtime processing.
 */
class NoteTrackEngine : public TrackEngine {
public:
    /**
     * @brief Constructs a NoteTrackEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] track Track model operated on by this component.
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    NoteTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        /**
         * @brief Returns the track engine.
         */
        TrackEngine(engine, model, track, linkedTrackEngine),
        /**
         * @brief Returns the note track.
         */
        _noteTrack(track.noteTrack())
    {
        reset();
    }

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Note; }

    /**
     * @brief Resets the NoteTrackEngine to its initial runtime state.
     */
    virtual void reset() override;
    /**
     * @brief Restarts the NoteTrackEngine while retaining persistent configuration.
     */
    virtual void restart() override;
    /**
     * @brief Advances the NoteTrackEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return Tick-processing result describing which outputs changed.
     */
    virtual TickResult tick(uint32_t tick) override;
    /**
     * @brief Updates the NoteTrackEngine for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    virtual void update(float dt) override;

    /**
     * @brief Applies the pending pattern change to this track engine.
     */
    virtual void changePattern() override;

    /**
     * @brief Processes a MIDI message for live monitoring and recording.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] message Message to process.
     */
    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;
    /**
     * @brief Clears transient MIDI-monitoring state.
     */
    virtual void clearMidiMonitoring() override;

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
    const NoteSequence &sequence() const { return *_sequence; }
    /**
     * @brief Reports whether active sequence.
     *
     * @param[in] sequence Sequence to inspect or modify.
     *
     * @return `true` if active sequence; otherwise `false`.
     */
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    /**
     * @brief Returns the current step.
     *
     * @return Zero-based index of the current sequence step, or the documented sentinel before a step is active.
     */
    int currentStep() const { return _currentStep; }
    /**
     * @brief Returns the current record step.
     *
     * @return Zero-based index of the step currently targeted by live recording.
     */
    int currentRecordStep() const { return _stepRecorder.stepIndex(); }

    /**
     * @brief Sets the monitor step.
     *
     * @param[in] index Zero-based monitor step index.
     */
    void setMonitorStep(int index);


private:
    /**
     * @brief Schedules events for the selected sequence step.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void triggerStep(uint32_t tick, uint32_t divisor);
    /**
     * @brief Records monitored input into the selected sequence step.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void recordStep(uint32_t tick, uint32_t divisor);
    /**
     * @brief Converts a MIDI note number to the internal note domain.
     *
     * @param[in] midiNote MIDI note number in the standard 0..127 domain.
     *
     * @return Internal note value corresponding to the MIDI note number.
     */
    int noteFromMidiNote(uint8_t midiNote) const;

    /**
     * @brief Returns the effective fill state after applying the note track's fill/mute policy.
     *
     * @return `true` when fill behavior is active for this note track.
     */
    bool fill() const {
        return (_noteTrack.fillMuted() || !TrackEngine::mute()) ? TrackEngine::fill() : false;
    }

    /**
     * @brief Persistent note-track model operated on by this engine.
     */
    NoteTrack &_noteTrack; ///< Persistent note-track model operated on by this engine.

    /**
     * @brief Timing and traversal metadata exposed to linked tracks.
     */
    TrackLinkData _linkData; ///< Timing and traversal metadata exposed to linked tracks.

    /**
     * @brief Currently active sequence.
     */
    NoteSequence *_sequence; ///< Currently active sequence.
    /**
     * @brief Sequence used when fill mode selects alternate pattern data.
     */
    const NoteSequence *_fillSequence; ///< Sequence used when fill mode selects alternate pattern data.

    /**
     * @brief Tick position used by free-running mode relative to its local phase origin.
     */
    uint32_t _freeRelativeTick; ///< Tick position used by free-running mode relative to its local phase origin.
    /**
     * @brief Traversal state for the active sequence.
     */
    SequenceState _sequenceState; ///< Traversal state for the active sequence.
    /**
     * @brief Zero-based index of the currently active sequence step.
     */
    int _currentStep; ///< Zero-based index of the currently active sequence step.
    /**
     * @brief Previous condition retained for comparison.
     */
    bool _prevCondition; ///< Result of the previous step-condition evaluation, used by chained conditions.

    /**
     * @brief Zero-based monitor step index; a negative/sentinel value represents no selection where applicable.
     */
    int _monitorStepIndex = -1; ///< Zero-based monitor step index; a negative/sentinel value represents no selection where applicable.

    /**
     * @brief History used to place and reconcile live-recorded note events.
     */
    RecordHistory _recordHistory; ///< History used to place and reconcile live-recorded note events.
    /**
     * @brief Whether monitor override is currently active.
     */
    bool _monitorOverrideActive = false; ///< Whether monitor override is currently active.
    /**
     * @brief Recorder that tracks the step targeted by live note recording.
     */
    StepRecorder _stepRecorder; ///< Recorder that tracks the step targeted by live note recording.

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
    float _cvOutput; ///< Current control-voltage output value in the engine output domain.
    /**
     * @brief Target control-voltage value used by slide/interpolation processing.
     */
    float _cvOutputTarget; ///< Target control-voltage value used by slide/interpolation processing.
    /**
     * @brief Whether slide is currently active.
     */
    bool _slideActive; ///< Whether slide is currently active.

    /**
     * @brief Identifies the exact future step already materialized by negative-offset look-ahead.
     */
    struct LookAheadReservation {
        /**
         * @brief Zero-based sequence-step index associated with this state/event.
         */
        int step = -1; ///< Zero-based future sequence-step index reserved by look-ahead; -1 when no reservation exists.
        /**
         * @brief Sequence traversal iteration associated with this state/reservation.
         */
        uint32_t iteration = 0; ///< Traversal iteration expected for the reserved future step.
        /**
         * @brief Generation used to invalidate speculative microtiming events after the timing plan changes.
         */
        uint32_t planningVersion = 0; ///< Generation used to invalidate speculative microtiming events after the timing plan changes.
        /**
         * @brief Whether this state/reservation currently contains usable data.
         */
        bool valid = false; ///< Whether this state/reservation currently contains usable data.
    };

    /**
     * @brief Identity of the future step already scheduled by negative microtiming look-ahead.
     */
    LookAheadReservation _lookAheadReservation; ///< Identity of the future step already scheduled by negative microtiming look-ahead.
    /**
     * @brief Generation used to invalidate speculative microtiming events after the timing plan changes.
     */
    uint32_t _planningVersion = 1; ///< Generation used to invalidate speculative microtiming events after the timing plan changes.
    /**
     * @brief Monotonic identifier assigned to newly scheduled gate generations.
     */
    uint32_t _gateGenerationCounter = 0; ///< Monotonic identifier assigned to newly scheduled gate generations.
    /**
     * @brief Generation identifier that currently owns the high gate output.
     */
    uint32_t _activeGateGeneration = 0; ///< Generation identifier that currently owns the high gate output.

    /**
     * @brief Describes one scheduled gate transition in the engine-tick domain.
     */
    struct Gate {
        /**
         * @brief Current absolute sequencer engine tick.
         */
        uint32_t tick; ///< Absolute engine tick at which this gate transition must be applied.
        /**
         * @brief Whether gate is true in the current state.
         */
        bool gate; ///< Target gate level for this scheduled transition.
        /**
         * @brief Generation identifier used to order or invalidate related runtime events.
         */
        uint32_t generation; ///< Generation identifier used to order or invalidate related runtime events.
        /**
         * @brief Generation used to invalidate speculative microtiming events after the timing plan changes.
         */
        uint32_t planningVersion; ///< Generation used to invalidate speculative microtiming events after the timing plan changes.
        /**
         * @brief Whether the event was scheduled by look-ahead and may be invalidated before its boundary.
         */
        bool speculative; ///< Whether the event was scheduled by look-ahead and may be invalidated before its boundary.
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
            if (a.tick != b.tick) {
                return a.tick < b.tick;
            }
            // At an identical engine tick, establish the new gate generation
            // before considering an older gate-off.  The generation check then
            // suppresses stale offs without creating a false low pulse.
            return a.gate && !b.gate;
        }
    };

    /**
     * @brief Chronologically ordered pending gate events.
     */
    SortedQueue<Gate, 32, GateCompare> _gateQueue; ///< Chronologically ordered pending gate events.

    /**
     * @brief Describes one scheduled CV update in the engine-tick domain.
     */
    struct Cv {
        /**
         * @brief Current absolute sequencer engine tick.
         */
        uint32_t tick; ///< Absolute engine tick at which this gate transition must be applied.
        /**
         * @brief Control-voltage value carried by this state/event.
         */
        float cv; ///< Control-voltage value carried by this state/event.
        /**
         * @brief Whether this CV event enables or continues slide interpolation.
         */
        bool slide; ///< Whether this CV event enables or continues slide interpolation.
        /**
         * @brief Generation used to invalidate speculative microtiming events after the timing plan changes.
         */
        uint32_t planningVersion; ///< Generation used to invalidate speculative microtiming events after the timing plan changes.
        /**
         * @brief Whether the event was scheduled by look-ahead and may be invalidated before its boundary.
         */
        bool speculative; ///< Whether the event was scheduled by look-ahead and may be invalidated before its boundary.
    };

    /**
     * @brief Defines the ordering relation used by the enclosing sorted event queue.
     */
    struct CvCompare {
        /**
         * @brief Compares two values according to this type's ordering or equality semantics.
         *
         * @param[in] a Left/first value used by the comparison or binary operation.
         * @param[in] b Right/second value used by the comparison or binary operation.
         *
         * @return `true` if operator(); otherwise `false`.
         */
        bool operator()(const Cv &a, const Cv &b) {
            return a.tick < b.tick;
        }
    };

    /**
     * @brief Chronologically ordered pending CV events.
     */
    SortedQueue<Cv, 32, CvCompare> _cvQueue; ///< Chronologically ordered pending CV events.
};
