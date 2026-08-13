/**
 * @file TrackEngine.h
 * @brief Declares the TrackEngine component used by the sequencer engine.
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

#include "Config.h"

#include "EngineState.h"
#include "MidiPort.h"

#include "model/Model.h"

#include "core/midi/MidiMessage.h"
#include "core/utils/EnumUtils.h"

#include <cstdint>

class Engine;
class SequenceState;

/**
 * @brief Exposes timing and traversal state that a linked track can follow.
 */
struct TrackLinkData {
    /**
     * @brief Runtime value representing divisor.
     */
    uint32_t divisor; ///< Effective source divisor, in sequencer-engine ticks per linked step.
    /**
     * @brief Runtime value representing relative tick.
     */
    uint32_t relativeTick; ///< Source-local tick position exposed to linked tracks.
    /**
     * @brief Traversal state for the active sequence.
     */
    SequenceState *sequenceState; ///< Traversal state for the active sequence.
};

#if CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) ASSERT(_actual_ == _expected_, "invalid track mode");
#else // CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) {}
#endif // CONFIG_ENABLE_SANITIZE

/**
 * @brief Defines the common runtime interface and shared state used by all sequencer track engines.
 */
class TrackEngine {
public:
    // Set of updates resulting from calling tick().
    /**
     * @brief Enumerates the supported tick result values.
     */
    enum TickResult {
        NoUpdate    = 0, ///< Selects the no update tick result.
        CvUpdate    = (1<<0), ///< Selects the cv update tick result.
        GateUpdate  = (1<<1), ///< Selects the gate update tick result.
    };

    /**
     * @brief Constructs a TrackEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] track Track model operated on by this component.
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    TrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        /**
         * @brief Returns the engine.
         */
        _engine(engine),
        /**
         * @brief Returns the model.
         */
        _model(model),
        /**
         * @brief Returns the track.
         */
        _track(track),
        /**
         * @brief Returns the track state.
         */
        _trackState(model.project().playState().trackState(track.trackIndex())),
        /**
         * @brief Returns the linked track engine.
         */
        _linkedTrackEngine(linkedTrackEngine)
    {
        changePattern();
    }

    /**
     * @brief Returns the track engine used as the link source.
     *
     * @return Pointer to the linked track engine; `nullptr` when no value is available.
     */
    const TrackEngine *linkedTrackEngine() const { return _linkedTrackEngine; }
    /**
     * @brief Sets the track engine used as the link source.
     *
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    void setLinkedTrackEngine(const TrackEngine *linkedTrackEngine) {
        _linkedTrackEngine = linkedTrackEngine;
    }

    template<typename T>
    /**
     * @brief Returns the as.
     *
     * @return Reference to the as.
     */
    const T &as() const {
        SANITIZE_TRACK_MODE(_track.trackMode(), trackMode());
        return *static_cast<const T *>(this);
    }

    template<typename T>
    /**
     * @brief Returns the as.
     *
     * @return Reference to the as.
     */
    T &as() {
        SANITIZE_TRACK_MODE(_track.trackMode(), trackMode());
        return *static_cast<T *>(this);
    }

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    virtual Track::TrackMode trackMode() const = 0;

    // sequencer control

    /**
     * @brief Resets the TrackEngine to its initial runtime state.
     */
    virtual void reset() = 0;
    /**
     * @brief Restarts the TrackEngine while retaining persistent configuration.
     */
    virtual void restart() = 0;
    /**
     * @brief Advances the TrackEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return Tick-processing result describing which outputs changed.
     */
    virtual TickResult tick(uint32_t tick) = 0;
    /**
     * @brief Updates the TrackEngine for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Applies the pending pattern change to this track engine.
     */
    virtual void changePattern() {}

    /**
     * @brief Processes MIDI input received by the engine.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if receive midi; otherwise `false`.
     */
    virtual bool receiveMidi(MidiPort port, const MidiMessage &message) { return false; }
    /**
     * @brief Processes a MIDI message for live monitoring and recording.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] message Message to process.
     */
    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) {}
    /**
     * @brief Clears transient MIDI-monitoring state.
     */
    virtual void clearMidiMonitoring() {}

    /**
     * @brief Returns timing/state data exposed to linked tracks.
     *
     * @return Pointer to the link data; `nullptr` when no value is available.
     */
    virtual const TrackLinkData *linkData() const { return nullptr; }

    // track output

    /**
     * @brief Reports whether the track produced visible or output activity.
     *
     * @return `true` if activity; otherwise `false`.
     */
    virtual bool activity() const = 0;
    /**
     * @brief Returns the current gate state for a track output channel.
     *
     * @param[in] index Zero-based gate-output channel index.
     *
     * @return `true` if gate output; otherwise `false`.
     */
    virtual bool gateOutput(int index) const = 0;
    /**
     * @brief Returns the current CV value for a track output channel.
     *
     * @param[in] index Zero-based CV-output channel index.
     *
     * @return Current control-voltage value for the addressed output channel.
     */
    virtual float cvOutput(int index) const = 0;

    /**
     * @brief Returns normalized progress through the active sequence.
     *
     * @return Normalized progress through the active sequence in the range 0..1.
     */
    virtual float sequenceProgress() const { return -1.f; }


    // helpers

    /**
     * @brief Reports whether this track is currently selected in the project.
     *
     * @return `true` if selected; otherwise `false`.
     */
    bool isSelected() const { return _model.project().selectedTrackIndex() == _track.trackIndex(); }

    /**
     * @brief Returns the swing.
     *
     * @return Configured swing amount in the model-defined percentage/range.
     */
    int swing() const { return _model.project().swing(); }

    /**
     * @brief Returns the pattern.
     *
     * @return Current pattern index.
     */
    int pattern() const { return _trackState.pattern(); }
    /**
     * @brief Returns the mute.
     *
     * @return `true` if mute; otherwise `false`.
     */
    bool mute() const { return _trackState.mute(); }
    /**
     * @brief Fills the target region or buffer using the current/supplied value.
     *
     * @return `true` if fill; otherwise `false`.
     */
    bool fill() const { return _trackState.fill(); }
    /**
     * @brief Fills amount.
     *
     * @return Current fill amount used by playback decisions.
     */
    int fillAmount() const { return _trackState.fillAmount(); }

protected:
    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
    /**
     * @brief Persistent sequencer model used by this component.
     */
    const Model &_model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Persistent track model operated on by this component.
     */
    Track &_track; ///< Persistent track model operated on by this component.
    /**
     * @brief Reference to track state owned by another component.
     */
    const PlayState::TrackState &_trackState; ///< Reference to track state owned by another component.
    /**
     * @brief Track engine supplying linked timing/state, or `nullptr` when unlinked.
     */
    const TrackEngine *_linkedTrackEngine; ///< Track engine supplying linked timing/state, or `nullptr` when unlinked.
};

ENUM_CLASS_OPERATORS(TrackEngine::TickResult)

#undef SANITIZE_TRACK_MODE
