/**
 * @file Scale.h
 * @brief Declares the Scale component used by the sequencer model.
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
#include "Types.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <algorithm>

#include <cstdint>
#include <cmath>

/**
 * @brief Stores and manipulates scale model data.
 */
class Scale {
public:
    /**
     * @brief Enumerates the supported format values.
     */
    enum Format {
        Short1, ///< Selects the short1 format.
        Short2, ///< Selects the short2 format.
        Long, ///< Selects the long format.
    };

    /**
     * @brief Constructs a Scale instance.
     *
     * @param[in] name Name or display string associated with the object.
     */
    Scale(const char *name) :
        /**
         * @brief Returns the display name.
         */
        _displayName(name)
    /**
     * @brief Pointer to the chromatic; `nullptr` denotes that no object is currently assigned.
     */
    {}

    /**
     * @brief Reports whether chromatic.
     *
     * @return `true` if chromatic; otherwise `false`.
     */
    virtual bool isChromatic() const = 0;

    /**
     * @brief Returns the display name of a note.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] rootNote Root note used by the scale/note operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    virtual void noteName(StringBuilder &str, int note, int rootNote, Format format = Long) const = 0;
    /**
     * @brief Returns note to volts.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return Control-voltage value, in volts, representing the note.
     */
    virtual float noteToVolts(int note) const = 0;
    /**
     * @brief Returns note from volts.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Note value derived/quantized from the supplied voltage.
     */
    virtual int noteFromVolts(float volts) const = 0;

    /**
     * @brief Returns the notes per octave.
     *
     * @return Number of scale degrees that make up one octave of the scale.
     */
    virtual int notesPerOctave() const = 0;

#ifdef FIX_BROKEN_SCALES
    /**
     * @brief Minimum supported octave.
     */
    static constexpr int MinOctave = -5; ///< Minimum supported octave.
    /**
     * @brief Maximum supported octave.
     */
    static constexpr int MaxOctave = 5; ///< Maximum supported octave.

    /**
     * @brief Returns the min note.
     *
     * @return Lowest note represented by this scale.
     */
    int minNote() const {
        return MinOctave * notesPerOctave();
    }

    /**
     * @brief Returns the max note.
     *
     * @return Highest note represented by this scale.
     */
    int maxNote() const {
        return MaxOctave * notesPerOctave();
    }

    /**
     * @brief Clamps note.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return Note value clamped to the supported note range.
     */
    int clampNote(int note) const {
        return clamp(note, minNote(), maxNote());
    }
#endif

    static int Count; ///< Total number of registered scales available through the scale registry.
    /**
     * @brief Returns get.
     *
     * @param[in] index Zero-based get index.
     *
     * @return Reference to the get.
     */
    static const Scale &get(int index);
    /**
     * @brief Returns name.
     *
     * @param[in] index Zero-based name index.
     *
     * @return Pointer to the name; `nullptr` when no value is available.
     */
    static const char *name(int index);

private:
    /**
     * @brief Returns the display name for display.
     *
     * @return Pointer to the display name; `nullptr` when no value is available.
     */
    const char *displayName() const { return _displayName; }

    /**
     * @brief Pointer to display name; `nullptr` denotes that no object/resource is assigned.
     */
    const char *_displayName; ///< Pointer to display name; `nullptr` denotes that no object/resource is assigned.
};


/**
 * @brief Stores and manipulates note scale model data.
 */
class NoteScale : public Scale {
public:
    /**
     * @brief Constructs a NoteScale instance.
     *
     * @param[in] name Name or display string associated with the object.
     * @param[in] chromatic Whether chromatic is enabled for this operation.
     * @param[in] noteCount Number of valid MIDI/note entries in the supplied array.
     * @param[in] notes Array or collection of note values consumed by the operation.
     */
    NoteScale(const char *name, bool chromatic, uint16_t noteCount, const uint16_t *notes) :
        /**
         * @brief Returns the scale.
         */
        Scale(name),
        /**
         * @brief Returns the chromatic.
         */
        _chromatic(chromatic),
        /**
         * @brief Returns the note count.
         */
        _noteCount(noteCount),
        /**
         * @brief Returns the notes.
         */
        _notes(notes)
    /**
     * @brief Pointer to the chromatic; `nullptr` denotes that no object is currently assigned.
     */
    {
    }

    /**
     * @brief Reports whether chromatic.
     *
     * @return `true` if chromatic; otherwise `false`.
     */
    bool isChromatic() const override {
        return _chromatic;
    }

    /**
     * @brief Returns the display name of a note.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] rootNote Root note used by the scale/note operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    void noteName(StringBuilder &str, int note, int rootNote, Format format) const override {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        bool printNote = format == Short1 || format == Long;
        bool printOctave = format == Short2 || format == Long;

        int octave = roundDownDivide(note, _noteCount);

        int noteIndex = 0;
        if (isChromatic()) {
            noteIndex = _notes[note - octave * _noteCount] / 128 + rootNote;
            while (noteIndex >= 12) {
                noteIndex -= 12;
                octave += 1;
            }
        } else {
            noteIndex = note - octave * _noteCount + 1;
        }

        if (printNote) {
            if (isChromatic()) {
                Types::printNote(str, noteIndex);
            } else {
                str(TXT_MODEL_GENERIC_VALUE, noteIndex);
            }
        }

        if (printOctave) {
            str(TXT_MODEL_OCTAVE, octave);
        }
    }

    /**
     * @brief Returns note to volts.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return Control-voltage value, in volts, representing the note.
     */
    float noteToVolts(int note) const override {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        int octave = roundDownDivide(note, _noteCount);
        int index = note - octave * _noteCount;
        return octave + _notes[index] * (1.f / 1536.f);
    }

    /**
     * @brief Returns note from volts.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Note value derived/quantized from the supplied voltage.
     */
    int noteFromVolts(float volts) const override {
        volts += 0.01f;
        int octave = std::floor(volts);
        float fractional = volts - octave;

        int index = -1;
        for (int i = 0; i < _noteCount; ++i) {
            if (fractional < _notes[i] * (1.f / 1536.f)) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _noteCount -1;
            --octave;
        }
#ifdef FIX_BROKEN_SCALES
        return clampNote(octave * _noteCount + index);
#else
        return octave * _noteCount + index;
#endif
    }

    /**
     * @brief Returns the notes per octave.
     *
     * @return Number of scale degrees that make up one octave of the scale.
     */
    int notesPerOctave() const override {
        return _noteCount;
    }

private:
    /**
     * @brief Whether chromatic is true in the current state.
     */
    bool _chromatic; ///< True when the scale is treated as chromatic rather than constrained to a note subset.
    /**
     * @brief Number of note items currently tracked or supported.
     */
    uint16_t _noteCount; ///< Number of note items currently tracked or supported.
    /**
     * @brief Pointer to notes; `nullptr` denotes that no object/resource is assigned.
     */
    const uint16_t *_notes; ///< Pointer to notes; `nullptr` denotes that no object/resource is assigned.
};

/**
 * @brief Stores and manipulates volt scale model data.
 */
class VoltScale : public Scale {
public:
    /**
     * @brief Constructs a VoltScale instance.
     *
     * @param[in] name Name or display string associated with the object.
     * @param[in] interval Interval or duration in the unit defined by the owning API.
     */
    VoltScale(const char *name, float interval) :
        /**
         * @brief Returns the scale.
         */
        Scale(name),
        /**
         * @brief Returns the interval.
         */
        _interval(interval)
    /**
     * @brief Pointer to the chromatic; `nullptr` denotes that no object is currently assigned.
     */
    {
    }

    /**
     * @brief Reports whether chromatic.
     *
     * @return `true` if chromatic; otherwise `false`.
     */
    bool isChromatic() const override {
        return false;
    }

    /**
     * @brief Returns the display name of a note.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] rootNote Root note used by the scale/note operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    void noteName(StringBuilder &str, int note, int rootNote, Format format) const override {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        switch (format) {
        case Short1:
            str(TXT_MODEL_NOTE_OCTAVE, std::abs(note * _interval));
            break;
        case Short2:
            str(TXT_MODEL_GENERIC_CHAR, note < 0 ? '-' : '+');
            break;
        case Long:
            str(TXT_MODEL_NOTE_LONG, note * _interval);
            break;
        }
    }

    /**
     * @brief Returns note to volts.
     *
     * @param[in] note Note value supplied to the operation.
     *
     * @return Control-voltage value, in volts, representing the note.
     */
    float noteToVolts(int note) const override {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        return note * _interval;
    }

    /**
     * @brief Returns note from volts.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Note value derived/quantized from the supplied voltage.
     */
    int noteFromVolts(float volts) const override {
#ifdef FIX_BROKEN_SCALES
        return clampNote(int(std::floor(volts / _interval)));
#else
        return int(std::floor(volts / _interval));
#endif
    }

    /**
     * @brief Returns the notes per octave.
     *
     * @return Number of scale degrees that make up one octave of the scale.
     */
    int notesPerOctave() const override {
        return std::max(1, int(std::round(1.f / _interval)));
    }

private:
    float _interval; ///< Equal-step interval in volts used to quantize the chromatic/equal-temperament scale.
};
