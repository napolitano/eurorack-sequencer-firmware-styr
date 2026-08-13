/**
 * @file MidiOutputEngine.h
 * @brief Declares the MidiOutputEngine component used by the sequencer engine.
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

#include "MidiPort.h"

#include "model/MidiConfig.h"
#include "model/MidiOutput.h"

#include <array>
#include <cstdint>

class Engine;
class Model;
class MidiMessage;

/**
 * @brief Implements midi output engine runtime processing.
 */
class MidiOutputEngine {
public:
    /**
     * @brief Constructs a MidiOutputEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     */
    MidiOutputEngine(Engine &engine, Model &model);

    /**
     * @brief Resets the MidiOutputEngine to its initial runtime state.
     */
    void reset();
    /**
     * @brief Updates the MidiOutputEngine for the current service cycle.
     *
     * @param[in] forceSendCC Whether force send cc is enabled for this operation.
     */
    void update(bool forceSendCC = false);

    /**
     * @brief Sends gate.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] gate Whether gate is enabled for this operation.
     */
    void sendGate(int trackIndex, bool gate);
    /**
     * @brief Sends slide.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] slide Whether slide is enabled for this operation.
     */
    void sendSlide(int trackIndex, bool slide);
    /**
     * @brief Sends cv.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] cv Control-voltage value in the engine/model voltage domain.
     */
    void sendCv(int trackIndex, float cv);

private:
    /**
     * @brief Stores output state used while the component is running.
     */
    struct OutputState {
        /**
         * @brief Enumerates the supported requests values.
         */
        enum Requests {
            NoteOn          = 1<<0, ///< Selects the note on requests.
            NoteOff         = 1<<1, ///< Selects the note off requests.
            ControlChange   = 1<<2, ///< Selects the control change requests.
            Slide           = 1<<3, ///< Selects the slide requests.
        };

        MidiOutput::Output::Event event = MidiOutput::Output::Event::None; ///< MIDI event type for which this output runtime state is valid.
        MidiTargetConfig target; ///< MIDI routing target configuration.

        /**
         * @brief Runtime value representing requests.
         */
        uint8_t requests; ///< Pending request bit mask/collection.
        /**
         * @brief Runtime value representing note.
         */
        int8_t note; ///< Note value represented by this state/step.
        /**
         * @brief Whether this CV event enables or continues slide interpolation.
         */
        int8_t slide; ///< Whether this CV event enables or continues slide interpolation.
        /**
         * @brief Runtime value representing velocity.
         */
        int8_t velocity; ///< MIDI/note velocity value.
        /**
         * @brief Runtime value representing control.
         */
        int8_t control; ///< Current MIDI/control value tracked by the converter.

        /**
         * @brief Currently active note.
         */
        int8_t activeNote; ///< Currently active note.

        /**
         * @brief Constructs a OutputState instance.
         */
        OutputState() { reset(); }

        /**
         * @brief Resets the OutputState to its initial runtime state.
         */
        void reset() {
            requests = 0;
            note = 60;
            slide = 0;
            velocity = 100;
            control = 0;

            activeNote = -1;
        };

        /**
         * @brief Sets the request.
         *
         * @param[in] request Request object/value to queue or process.
         */
        void setRequest(uint8_t request) { requests |= request; }
        /**
         * @brief Clears request.
         *
         * @param[in] request Request object/value to queue or process.
         */
        void clearRequest(uint8_t request) { requests &= ~request; }
        /**
         * @brief Reports whether the OutputState has request.
         *
         * @param[in] request Request object/value to queue or process.
         *
         * @return `true` if request; otherwise `false`.
         */
        bool hasRequest(uint8_t request) { return requests & request; }
    };

    /**
     * @brief Resets output.
     *
     * @param[in] outputIndex Zero-based output index.
     */
    void resetOutput(int outputIndex);

    /**
     * @brief Sends a MIDI message through the requested output port.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     */
    void sendMidi(MidiPort port, const MidiMessage &message);

    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
    /**
     * @brief Reference to midi output owned by another component.
     */
    const MidiOutput &_midiOutput; ///< Reference to midi output owned by another component.
    /**
     * @brief Fixed-capacity storage for output states.
     */
    std::array<OutputState, CONFIG_MIDI_OUTPUT_COUNT> _outputStates; ///< Runtime state for each configured MIDI output slot.
    /**
     * @brief Most recently observed send cc ticks.
     */
    uint32_t _lastSendCCTicks = 0; ///< Engine tick of the previous periodic Control Change transmission, used to rate-limit CC output.
};
