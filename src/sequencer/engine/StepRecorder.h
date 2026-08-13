/**
 * @file StepRecorder.h
 * @brief Declares the StepRecorder component used by the sequencer engine.
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

#include "model/NoteSequence.h"
#include "core/midi/MidiMessage.h"

#include <functional>

/**
 * @brief Tracks the step currently targeted by realtime Note recording.
 */
class StepRecorder {
public:
    /**
     * @brief Starts the StepRecorder or operation.
     *
     * @param[in] sequence Sequence to inspect or modify.
     */
    void start(const NoteSequence &sequence) {
        _stepIndex = sequence.firstStep();
        _pressedStepIndex = -1;
        _pressedNote = -1;
    }

    /**
     * @brief Stops the StepRecorder or operation.
     */
    void stop() {
        _stepIndex = -1;
    }

    /**
     * @brief Enables d.
     *
     * @return `true` if enabled; otherwise `false`.
     */
    bool enabled() {
        return _stepIndex != -1;
    }

    /**
     * @brief Returns the step index.
     *
     * @return Zero-based step index.
     */
    int stepIndex() const {
        return _stepIndex;
    }

    /**
     * @brief Sets the step index.
     *
     * @param[in] stepIndex Zero-based step index.
     */
    void setStepIndex(int stepIndex) {
        _stepIndex = stepIndex;
    }

    /**
     * @brief Processes the supplied input for this component.
     *
     * @param[in] message Message to process.
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] noteFromMidiNote Internal note value converted from the incoming MIDI note.
     */
    void process(const MidiMessage &message, NoteSequence &sequence, std::function<int(int)> noteFromMidiNote) {
        if (message.isNoteOn()) {
            // record to step
            auto &step = sequence.step(_stepIndex);
            step.setGate(true);
            step.setLength(NoteSequence::Length::Max / 2);
            step.setNote(noteFromMidiNote(message.note()));

            // remember last edited step
            _pressedNote = message.note();
            _pressedStepIndex = _stepIndex;
        } else if (message.isNoteOff()) {
            if (message.note() == _pressedNote) {
                _pressedNote = -1;
                _pressedStepIndex = -1;

                // move to next step
                ++_stepIndex;
                if (_stepIndex > sequence.lastStep()) {
                    _stepIndex = sequence.firstStep();
                }
            }
        } else if (message.isPitchBend()) {
            // tag slide
            if (_pressedStepIndex != -1) {
                sequence.step(_pressedStepIndex).setSlide(true);
            }
        } else if (message.isControlChange() && message.controlNumber() == 1) {
            // tag tie
            if (_pressedStepIndex != -1) {
                sequence.step(_pressedStepIndex).setLength(NoteSequence::Length::Max);
            }
        }
    }

private:
    /**
     * @brief Zero-based step index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _stepIndex = -1; ///< Zero-based step index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Zero-based pressed step index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _pressedStepIndex = -1; ///< Zero-based pressed step index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Runtime value representing pressed note.
     */
    int8_t _pressedNote = -1; ///< Currently pressed/held MIDI note or sentinel.
};
