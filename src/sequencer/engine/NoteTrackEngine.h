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

class NoteTrackEngine : public TrackEngine {
public:
    NoteTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(engine, model, track, linkedTrackEngine),
        _noteTrack(track.noteTrack())
    {
        reset();
    }

    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::Note; }

    virtual void reset() override;
    virtual void restart() override;
    virtual TickResult tick(uint32_t tick) override;
    virtual void update(float dt) override;

    virtual void changePattern() override;

    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) override;
    virtual void clearMidiMonitoring() override;

    virtual const TrackLinkData *linkData() const override { return &_linkData; }

    virtual bool activity() const override { return _activity; }
    virtual bool gateOutput(int index) const override { return _gateOutput; }
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

    const NoteSequence &sequence() const { return *_sequence; }
    bool isActiveSequence(const NoteSequence &sequence) const { return &sequence == _sequence; }

    int currentStep() const { return _currentStep; }
    int currentRecordStep() const { return _stepRecorder.stepIndex(); }

    void setMonitorStep(int index);


private:
    void triggerStep(uint32_t tick, uint32_t divisor);
    void recordStep(uint32_t tick, uint32_t divisor);
    int noteFromMidiNote(uint8_t midiNote) const;

    bool fill() const {
        return (_noteTrack.fillMuted() || !TrackEngine::mute()) ? TrackEngine::fill() : false;
    }

    NoteTrack &_noteTrack;

    TrackLinkData _linkData;

    NoteSequence *_sequence;
    const NoteSequence *_fillSequence;

    uint32_t _freeRelativeTick;
    SequenceState _sequenceState;
    int _currentStep;
    bool _prevCondition;

    int _monitorStepIndex = -1;

    RecordHistory _recordHistory;
    bool _monitorOverrideActive = false;
    StepRecorder _stepRecorder;

    bool _activity;
    bool _gateOutput;
    float _cvOutput;
    float _cvOutputTarget;
    bool _slideActive;

    struct LookAheadReservation {
        int step = -1;
        uint32_t iteration = 0;
        uint32_t planningVersion = 0;
        bool valid = false;
    };

    LookAheadReservation _lookAheadReservation;
    uint32_t _planningVersion = 1;
    uint32_t _gateGenerationCounter = 0;
    uint32_t _activeGateGeneration = 0;

    struct Gate {
        uint32_t tick;
        bool gate;
        uint32_t generation;
        uint32_t planningVersion;
        bool speculative;
    };

    struct GateCompare {
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

    SortedQueue<Gate, 32, GateCompare> _gateQueue;

    struct Cv {
        uint32_t tick;
        float cv;
        bool slide;
        uint32_t planningVersion;
        bool speculative;
    };

    struct CvCompare {
        bool operator()(const Cv &a, const Cv &b) {
            return a.tick < b.tick;
        }
    };

    SortedQueue<Cv, 32, CvCompare> _cvQueue;
};
