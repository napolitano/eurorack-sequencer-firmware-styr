/**
 * @file ArpeggiatorEngine.h
 * @brief Declares the ArpeggiatorEngine component used by the sequencer engine.
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

#include "SortedQueue.h"

#include "model/Arpeggiator.h"

#include <array>

#include <cstdint>

/**
 * @brief Implements arpeggiator engine runtime processing.
 */
class ArpeggiatorEngine {
public:
    /**
     * @brief Describes one event handled or queued by the enclosing component.
     */
    struct Event {
        /**
         * @brief Enumerates the supported enumeration values.
         */
        enum {
            NoteOn, ///< Selects the note on option.
            NoteOff, ///< Selects the note off option.
        } action;
        /**
         * @brief Current absolute sequencer engine tick.
         */
        uint32_t tick; ///< Current absolute sequencer engine tick.
        /**
         * @brief Runtime value representing note.
         */
        uint8_t note; ///< Note value represented by this state/step.
        /**
         * @brief Runtime value representing velocity.
         */
        uint8_t velocity; ///< MIDI/note velocity value.
    };

    /**
     * @brief Constructs a ArpeggiatorEngine instance.
     *
     * @param[in] arpeggiator Arpeggiator state/configuration used to generate the next note.
     */
    ArpeggiatorEngine(const Arpeggiator &arpeggiator);

    /**
     * @brief Resets the ArpeggiatorEngine to its initial runtime state.
     */
    void reset();

    /**
     * @brief Processes a note-on event.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void noteOn(int note);
    /**
     * @brief Processes a note-off event.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void noteOff(int note);

    /**
     * @brief Advances the ArpeggiatorEngine for one sequencer engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] swing Swing setting in the model-defined range.
     */
    void tick(uint32_t tick, int swing);

    /**
     * @brief Returns get event.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     * @param[in] event Event to process.
     *
     * @return `true` if get event; otherwise `false`.
     */
    bool getEvent(uint32_t tick, Event &event);

private:
    /**
     * @brief Adds note.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void addNote(int note);
    /**
     * @brief Removes note.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void removeNote(int note);
    /**
     * @brief Returns note index from order.
     *
     * @param[in] order Ordering mode or ordinal value used for note traversal.
     *
     * @return Note/voice index selected by the configured ordering.
     */
    int noteIndexFromOrder(int order);
    /**
     * @brief Formats the notes into the supplied string builder/output.
     */
    void printNotes();

    /**
     * @brief Advances step.
     */
    void advanceStep();
    /**
     * @brief Advances octave.
     */
    void advanceOctave();

    /**
     * @brief Maximum supported notes.
     */
    static constexpr int MaxNotes = 8; ///< Maximum supported notes.

    /**
     * @brief Reference to arpeggiator owned by another component.
     */
    const Arpeggiator &_arpeggiator; ///< Reference to arpeggiator owned by another component.

    /**
     * @brief Zero-based step index; a negative/sentinel value represents no selection where applicable.
     */
    int _stepIndex; ///< Zero-based step index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Zero-based note index; a negative/sentinel value represents no selection where applicable.
     */
    int _noteIndex; ///< Zero-based note index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Runtime value representing note order.
     */
    uint32_t _noteOrder; ///< Voice/note ordering state.
    /**
     * @brief Runtime value representing octave.
     */
    int8_t _octave; ///< Octave state used by `ArpeggiatorEngine` to preserve its current configuration or runtime progress.
    /**
     * @brief Runtime value representing octave direction.
     */
    int8_t _octaveDirection; ///< Direction used when traversing arpeggiator octaves.

    /**
     * @brief Stores one note and its associated traversal/runtime metadata.
     */
    struct Note {
        /**
         * @brief Runtime value representing note.
         */
        uint8_t note; ///< Note value represented by this state/step.
        /**
         * @brief Runtime value representing order.
         */
        uint32_t order; ///< Current ordering index/state.
    };

    /**
     * @brief Fixed-capacity storage for notes.
     */
    std::array<Note, MaxNotes> _notes; ///< Held/source notes available to the arpeggiator, kept in its traversal order.
    /**
     * @brief Number of note items currently tracked or supported.
     */
    int8_t _noteCount; ///< Number of note items currently tracked or supported.
    /**
     * @brief Number of note hold items currently tracked or supported.
     */
    int8_t _noteHoldCount; ///< Number of note hold items currently tracked or supported.

    /**
     * @brief Defines the ordering relation used by the enclosing sorted event queue.
     */
    struct EventCompare {
        /**
         * @brief Compares two values according to this type's ordering or equality semantics.
         *
         * @param[in] a Left/first value used by the comparison or binary operation.
         * @param[in] b Right/second value used by the comparison or binary operation.
         *
         * @return `true` if operator(); otherwise `false`.
         */
        bool operator()(const Event &a, const Event &b) {
            return a.tick < b.tick;
        }
    };

    /**
     * @brief Chronologically ordered queue storing event queue.
     */
    SortedQueue<Event, 16, EventCompare> _eventQueue; ///< Chronologically ordered queue storing event queue.
};
