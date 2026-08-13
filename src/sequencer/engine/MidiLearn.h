/**
 * @file MidiLearn.h
 * @brief Declares the MidiLearn component used by the sequencer engine.
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

#include "MidiPort.h"

#include "core/midi/MidiMessage.h"

#include <array>
#include <functional>

/**
 * @brief Learns MIDI source assignments from incoming messages and reports matched results.
 */
class MidiLearn {
public:
    /**
     * @brief Enumerates the supported event values.
     */
    enum class Event : uint8_t {
        ControlAbsolute, ///< Selects the control absolute event.
        ControlRelative, ///< Selects the control relative event.
        PitchBend, ///< Selects the pitch bend event.
        Note, ///< Selects the note event.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Describes the MIDI source learned from an incoming message.
     */
    struct Result {
        MidiPort port; ///< Port state owned by this component.
        uint8_t channel; ///< Runtime value representing channel.
        Event event; ///< Event state owned by this component.
        /**
         * @brief Stores the control number state used by this component.
         */
        union {
            uint8_t controlNumber;
            uint8_t note;
        };

        /**
         * @brief Compares two learned MIDI-source results for value equality.
         * @param[in] other Learned result to compare with this instance.
         * @return `true` when port, channel, event, and payload fields match; otherwise `false`.
         */
        bool operator==(const Result &other) const {
            return
                port == other.port &&
                channel == other.channel &&
                event == other.event &&
                controlNumber == other.controlNumber;
        }
    };

    typedef std::function<void(const Result &result)> ResultCallback;

    /**
     * @brief Constructs a MidiLearn instance.
     */
    MidiLearn();

    /**
     * @brief Starts the MidiLearn or operation.
     *
     * @param[in] callback Callback invoked by the operation.
     */
    void start(ResultCallback callback);
    /**
     * @brief Stops the MidiLearn or operation.
     */
    void stop();

    /**
     * @brief Reports whether active.
     *
     * @return `true` if active; otherwise `false`.
     */
    bool isActive() const { return _callback != nullptr; }

    /**
     * @brief Processes MIDI input received by the engine.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     */
    void receiveMidi(MidiPort port, const MidiMessage &message);

private:
    /**
     * @brief Resets the MidiLearn to its initial runtime state.
     */
    void reset();
    /**
     * @brief Executes emit result.
     *
     * @param[in] result Result used by the operation.
     */
    void emitResult(Result result);

    ResultCallback _callback; ///< Callback invoked for callback events.

    MidiPort _port; ///< Port state owned by this component.
    int8_t _channel; ///< Runtime value representing channel.
    int8_t _controlNumber; ///< Runtime value representing control number.
    int8_t _lastControlNumber; ///< Most recently observed control number.
    int8_t _note; ///< Runtime value representing note.

    Result _lastResult; ///< Most recently observed result.

    std::array<uint8_t, size_t(Event::Last)> _eventCounters; ///< Per-event occurrence counters used while evaluating MIDI-learn candidates.
};
