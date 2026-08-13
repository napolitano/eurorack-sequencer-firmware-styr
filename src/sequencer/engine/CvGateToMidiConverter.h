/**
 * @file CvGateToMidiConverter.h
 * @brief Declares the CvGateToMidiConverter component used by the sequencer engine.
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

#include "core/midi/MidiMessage.h"
#include "core/math/Math.h"

#include <functional>

/**
 * @brief Converts CV pitch and gate transitions into MIDI note messages.
 */
class CvGateToMidiConverter {
public:
    /**
     * @brief Constructs a CvGateToMidiConverter instance.
     */
    CvGateToMidiConverter() {
        reset();
    }

    /**
     * @brief Resets the CvGateToMidiConverter to its initial runtime state.
     */
    void reset() {
        _lastGateOff = os::ticks();
        _gate = 0;
        _note = -1;
    }

    /**
     * @brief Converts the supplied input representation to the target representation.
     *
     * @param[in] pitchCv Pitch control voltage in volts or the engine CV domain defined by the converter.
     * @param[in] gateCv Gate control voltage or normalized gate value used by the converter.
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] callback Callback invoked by the operation.
     */
    void convert(float pitchCv, float gateCv, uint8_t channel, std::function<void(const MidiMessage &message)> callback) {
        int8_t note = clamp(60 + int(std::floor(pitchCv * 12.f + 0.5f)), 0, 127);

        if (_gate) {
            if (gateCv < 2.f) {
                // gate off
                if (_note != -1) {
                    callback(MidiMessage::makeNoteOff(channel, _note, 0));
                    _note = -1;
                }
                _gate = 0;
                _lastGateOff = os::ticks();
            } else if (note != _note) {
                // legato note change
                callback(MidiMessage::makeNoteOn(channel, note, 127));
                callback(MidiMessage::makeNoteOff(channel, _note, 0));
                _note = note;
            }
        } else {
            if (gateCv > 3.f) {
                if (os::ticks() - _lastGateOff >= GateOnDelay) {
                    // gate on
                    callback(MidiMessage::makeNoteOn(channel, note, 127));
                    _gate = 1;
                    _note = note;
                }
            } else {
                _lastGateOff = os::ticks();
            }
        }
    }

private:
    /**
     * @brief Returns the ms.
     *
     * @return The ms value.
     */
    static constexpr uint32_t GateOnDelay = os::time::ms(5); ///< Minimum low time required before accepting a new gate-on edge, expressed in platform ticks. ///< Minimum low time required before accepting a new gate-on edge, expressed in platform ticks.

    /**
     * @brief Most recently observed gate off.
     */
    uint32_t _lastGateOff; ///< Engine/system tick of the most recent gate-off edge, used by gate-to-MIDI timing logic.
    /**
     * @brief Runtime value representing gate.
     */
    uint8_t _gate; ///< Current gate state/value.
    /**
     * @brief Runtime value representing note.
     */
    int8_t _note; ///< Note value represented by this state/step.
};
