/**
 * @file MidiCvTrackEngine.h
 * @brief Declares the MidiCvTrackEngine component used by the sequencer engine.
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
#include "ArpeggiatorEngine.h"

#include "model/Track.h"

/**
 * @brief Implements midi cv track engine runtime processing.
 */
class MidiCvTrackEngine : public TrackEngine {
public:
    /**
     * @brief Constructs a MidiCvTrackEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] track Track model operated on by this component.
     * @param[in] linkedTrackEngine Optional linked track engine; `nullptr` when the track is not linked.
     */
    MidiCvTrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        /**
         * @brief Returns the track engine.
         */
        TrackEngine(engine, model, track, linkedTrackEngine),
        /**
         * @brief Returns the midi cv track.
         */
        _midiCvTrack(track.midiCvTrack()),
        /**
         * @brief Returns the arpeggiator engine.
         */
        _arpeggiatorEngine(_midiCvTrack.arpeggiator())
    {
        reset();
    }

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    virtual Track::TrackMode trackMode() const override { return Track::TrackMode::MidiCv; }

    /**
     * @brief Resets the MidiCvTrackEngine to its initial runtime state.
     */
    virtual void reset() override;
    /**
     * @brief Restarts the MidiCvTrackEngine while retaining persistent configuration.
     */
    virtual void restart() override;
    /**
     * @brief Advances the MidiCvTrackEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return Tick-processing result describing which outputs changed.
     */
    virtual TickResult tick(uint32_t tick) override;
    /**
     * @brief Updates the MidiCvTrackEngine for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    virtual void update(float dt) override;

    /**
     * @brief Processes MIDI input received by the engine.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if receive midi; otherwise `false`.
     */
    virtual bool receiveMidi(MidiPort port, const MidiMessage &message) override;

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

private:
    /**
     * @brief Number of voice entries supported by the implementation.
     */
    static constexpr size_t VoiceCount = 8; ///< Number of voice entries supported by the implementation.
    /**
     * @brief Retrigger delay constant used by this component.
     */
    static constexpr int RetriggerDelay = 2; ///< Engine-tick delay inserted before retriggering a MIDI-CV gate.

    /**
     * @brief Stores runtime allocation and output state for one MIDI-to-CV voice.
     */
    struct Voice {
        /**
         * @brief Runtime value representing ticks.
         */
        uint32_t ticks = 0; ///< Monotonic system tick counter maintained by the bootloader.
        /**
         * @brief Runtime value representing note.
         */
        uint8_t note = 60; ///< Note value represented by this state/step.
        /**
         * @brief Runtime value representing velocity.
         */
        uint8_t velocity = 0; ///< MIDI/note velocity value.
        /**
         * @brief Runtime value representing pressure.
         */
        uint8_t pressure = 0; ///< Current MIDI/channel pressure value.
        /**
         * @brief Runtime value representing output.
         */
        int8_t output = -1; ///< Current computed output value/state.

        /**
         * @brief Resets the Voice to its initial runtime state.
         */
        void reset() {
            ticks = 0;
            output = -1;
        }

        /**
         * @brief Releases the supplied resource or object.
         */
        void release() { ticks = 0; }

        /**
         * @brief Reports whether active.
         *
         * @return `true` if active; otherwise `false`.
         */
        bool isActive() const { return ticks != 0; }
        /**
         * @brief Reports whether allocated.
         *
         * @return `true` if allocated; otherwise `false`.
         */
        bool isAllocated() const { return output != -1; }
    };

    /**
     * @brief Refreshes activity indicators/state.
     */
    void updateActivity();

    /**
     * @brief Updates arpeggiator state from track/model changes.
     */
    void updateArpeggiator();
    /**
     * @brief Advances the arpeggiator for the current tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     */
    void tickArpeggiator(uint32_t tick);

    /**
     * @brief Returns note to cv.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return CV value representing the note.
     */
    float noteToCv(int note) const;
    /**
     * @brief Returns value to cv.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     *
     * @return CV value corresponding to the source value.
     */
    float valueToCv(int value) const;
    /**
     * @brief Returns pitch bend to cv.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     *
     * @return CV offset corresponding to the MIDI pitch-bend value.
     */
    float pitchBendToCv(int value) const;

    /**
     * @brief Resets voices.
     */
    void resetVoices();

    /**
     * @brief Adds voice.
     *
     * @param[in] note Note value supplied to the operation.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     */
    void addVoice(int note, int velocity);
    /**
     * @brief Removes voice.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void removeVoice(int note);
    /**
     * @brief Finds voice.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return Pointer to the find voice; `nullptr` when no value is available.
     */
    Voice *findVoice(int note);

    /**
     * @brief Orders active MIDI/CV voices according to note-priority rules.
     */
    void sortVoices();
    /**
     * @brief Formats the voices into the supplied string builder/output.
     */
    void printVoices();

    /**
     * @brief Reference to midi cv track owned by another component.
     */
    const MidiCvTrack &_midiCvTrack; ///< Reference to midi cv track owned by another component.

    ArpeggiatorEngine _arpeggiatorEngine; ///< Runtime arpeggiator that converts the MIDI-CV track chord/input state into note events.
    /**
     * @brief Whether arpeggiator is enabled.
     */
    bool _arpeggiatorEnabled; ///< Whether arpeggiator is enabled.
    /**
     * @brief Runtime value representing arpeggiator time.
     */
    float _arpeggiatorTime; ///< Time accumulator used by arpeggiator scheduling.
    /**
     * @brief Runtime value representing arpeggiator tick.
     */
    uint32_t _arpeggiatorTick; ///< Current arpeggiator tick counter.

    /**
     * @brief Fixed-capacity storage for voices.
     */
    std::array<Voice, VoiceCount> _voices; ///< Runtime state for each available MIDI-to-CV voice.
    /**
     * @brief Fixed-capacity storage for voice by output.
     */
    std::array<int8_t, VoiceCount> _voiceByOutput; ///< Mapping from CV/gate output slot to assigned voice index; negative entries denote no assignment.
    /**
     * @brief Next output scheduled or expected by this component.
     */
    int8_t _nextOutput; ///< Next output scheduled or expected by this component.

    /**
     * @brief Whether this track/component produced activity during the current processing interval.
     */
    bool _activity; ///< Whether this track/component produced activity during the current processing interval.

    /**
     * @brief Runtime value representing pitch bend.
     */
    int16_t _pitchBend; ///< Current MIDI pitch-bend value applied to voice pitch CV.
    /**
     * @brief Runtime value representing channel pressure.
     */
    uint8_t _channelPressure; ///< Current MIDI channel-pressure value used by configured modulation output.

    // slides for pitch, only valid in monophonic mode
    /**
     * @brief Whether slide is currently active.
     */
    bool _slideActive; ///< Whether slide is currently active.
    /**
     * @brief Runtime value representing pitch cv output target.
     */
    float _pitchCvOutputTarget; ///< Target pitch CV after note, transpose and pitch-bend processing.
    /**
     * @brief Runtime value representing pitch cv output.
     */
    float _pitchCvOutput; ///< Current pitch CV after any configured slide/slew toward the target.
};
