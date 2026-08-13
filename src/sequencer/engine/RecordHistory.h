/**
 * @file RecordHistory.h
 * @brief Declares the RecordHistory component used by the sequencer engine.
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

#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include <array>
#include <algorithm>

#include <cstdint>
#include <cinttypes>

/**
 * @brief Retains recent recorded note/gate events for correlation during step recording.
 */
class RecordHistory {
public:
    /**
     * @brief Enumerates the supported type values.
     */
    enum class Type : uint8_t {
        NoteOn, ///< Selects the note on type.
        NoteOff, ///< Selects the note off type.
    };

    /**
     * @brief Describes one event handled or queued by the enclosing component.
     */
    struct Event {
        /**
         * @brief Current absolute sequencer engine tick.
         */
        uint32_t tick; ///< Current absolute sequencer engine tick.
        Type type; ///< Selected type that determines how `RecordHistory` interprets or renders its data.
        /**
         * @brief Runtime value representing note.
         */
        int8_t note; ///< Note value represented by this state/step.
    };

    /**
     * @brief Constructs a RecordHistory instance.
     */
    RecordHistory() {
        clear();
    }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _activeNote = -1;
        _size = 0;
        _write = 0;
    }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const { return _size; }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] note Note value supplied to the operation.
     */
    void write(uint32_t tick, Type type, int note) {
        if (note < 0 || note > 127) {
            return;
        }

        switch (type) {
        case Type::NoteOn:
            if (_activeNote >= 0 && _activeNote != note) {
                write({ tick, Type::NoteOff, _activeNote });
            }
            _activeNote = note;
            write({ tick, Type::NoteOn, int8_t(note) });
            break;
        case Type::NoteOff:
            if (_activeNote == note) {
                _activeNote = -1;
                write({ tick, Type::NoteOff, int8_t(note) });
            }
            break;
        }
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] message Message to process.
     */
    void write(uint32_t tick, const MidiMessage &message) {
        if (message.isNoteOn()) {
            write(tick, Type::NoteOn, message.note());
        } else if (message.isNoteOff()) {
            write(tick, Type::NoteOff, message.note());
        }
    }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] index Zero-based component/element index.
     *
     * @return Reference to the operator[].
     */
    const Event &operator[](int index) const {
        return _events[(_write + index - _size + _events.size()) % _events.size()];
    }

    /**
     * @brief Reports whether note active.
     *
     * @return `true` if note active; otherwise `false`.
     */
    bool isNoteActive() const {
        return _activeNote >= 0;
    }

    /**
     * @brief Returns the active note.
     *
     * @return Currently recorded/held MIDI note, or the model sentinel when none is active.
     */
    int activeNote() const {
        return _activeNote;
    }

private:
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] event Event to process.
     */
    void write(const Event &event) {
        _events[_write] = event;
        _write = (_write + 1) % _events.size();
        _size = std::min(_events.size(), _size + 1);
    }

    /**
     * @brief Currently active note.
     */
    int8_t _activeNote; ///< Currently active note.
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    size_t _size; ///< Size of the associated data in bytes/elements as defined by this type.
    /**
     * @brief Runtime value representing write.
     */
    size_t _write; ///< Write cursor/index used by the ring buffer.
    /**
     * @brief Fixed-capacity storage for events.
     */
    std::array<Event, 4> _events; ///< Four-entry rolling history used to match newly recorded events with recent note/gate activity.
};
