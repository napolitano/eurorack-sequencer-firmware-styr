/**
 * @file TimeSignature.h
 * @brief Declares the TimeSignature component used by the sequencer model.
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
#include "Serialize.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

/**
 * @brief Stores and manipulates time signature model data.
 */
class TimeSignature {
public:
    /**
     * @brief Constructs a TimeSignature instance.
     */
    TimeSignature() = default;

    /**
     * @brief Constructs a TimeSignature instance.
     *
     * @param[in] beats Number of beats per measure represented by the time signature.
     * @param[in] note Note value supplied to the operation.
     */
    TimeSignature(int beats, int note) {
        setNote(note);
        setBeats(beats);
    }

    /**
     * @brief Returns the beats.
     *
     * @return Time-signature numerator in beats per measure.
     */
    int beats() const { return _beats; }
    /**
     * @brief Sets the beats.
     *
     * @param[in] beats Number of beats per measure represented by the time signature.
     */
    void setBeats(int beats) {
        _beats = clamp(beats, 1, _note * 2);
    }

    /**
     * @brief Returns the note.
     *
     * @return Note value encoded by the message or model object.
     */
    int note() const { return _note; }
    /**
     * @brief Sets the note.
     *
     * @param[in] note Note value supplied to the operation.
     */
    void setNote(int note) {
        auto it = std::find(validNotes.begin(), validNotes.end(), uint8_t(note));
        if (it == validNotes.end()) {
            _note = DefaultNote;
        } else {
            _note = *it;
        }
        setBeats(beats());
    }

    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void edit(int value, bool shift) {
        auto toIndex = [this] () {
            int index = 0;
            for (auto it = validNotes.begin(); *it < note() && it < validNotes.end(); ++it) {
                index += *it * 2;
            }
            return index + beats() - 1;
        };

        auto fromIndex = [this] (int index) {
            auto it = validNotes.begin();
            while (index >= *it * 2 && it < validNotes.end() - 1) {
                index -= *it * 2;
                ++it;
            }
            setNote(*it);
            setBeats(index + 1);
        };

        if (shift) {
            auto it = std::find(validNotes.begin(), validNotes.end(), _note);
            it += value;
            if (it >= validNotes.begin() && it < validNotes.end()) {
                setNote(*it);
            }
        } else {
            fromIndex(toIndex() + value);
        }
    }

    /**
     * @brief Writes the object representation to the supplied output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void print(StringBuilder &str) const {
        str(TXT_MODEL_BEATS_STEPS, beats(), note());
    }

    /**
     * @brief Returns the note divisor.
     *
     * @return Sequencer tick divisor represented by one beat of the configured time signature.
     */
    uint32_t noteDivisor() const {
        return (CONFIG_PPQN * 4) / note();
    }

    /**
     * @brief Returns the measure divisor.
     *
     * @return Sequencer tick divisor represented by one complete measure.
     */
    uint32_t measureDivisor() const {
        return beats() * noteDivisor();
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const {
        writer.write(_beats);
        writer.write(_note);
    }

    /**
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader) {
        reader.read(_beats);
        reader.read(_note);
    }

private:
    /**
     * @brief Valid notes constant used by this component.
     */
    static const std::array<uint8_t, 6> validNotes; ///< Supported time-signature denominator values.

    /**
     * @brief Default beats constant used by this component.
     */
    static constexpr int DefaultBeats = 4; ///< Default number of beats per measure.
    /**
     * @brief Default note constant used by this component.
     */
    static constexpr int DefaultNote = 4; ///< Default beat-unit denominator for a new time signature.

    uint8_t _beats = DefaultBeats; ///< Number of beats in one measure; initialized to the 4/4 default numerator.
    uint8_t _note = DefaultNote; ///< Beat-unit denominator of the time signature; constrained to `validNotes`.
};
