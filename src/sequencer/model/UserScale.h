/**
 * @file UserScale.h
 * @brief Declares the UserScale component used by the sequencer model.
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

#include "Scale.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "FileDefs.h"

#include "core/math/Math.h"
#include "core/utils/StringUtils.h"

#include <array>

#include <cstdint>

/**
 * @brief Stores and manipulates user scale model data.
 */
class UserScale : public Scale {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Name length constant used by this component.
     */
    static constexpr size_t NameLength = FileHeader::NameLength; ///< Maximum user-scale name length inherited from the fixed-width serialized file-header field.

    typedef std::array<UserScale, CONFIG_USER_SCALE_COUNT> Array;
    typedef std::array<int16_t, CONFIG_USER_SCALE_SIZE> ItemArray;

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Chromatic, ///< Selects the chromatic mode.
        Voltage, ///< Selects the voltage mode.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the mode name; `nullptr` when no value is available.
     */
    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Chromatic:   return TXT_LIST_LABEL_SCALE_CHROMATIC;
        case Mode::Voltage:     return TXT_LIST_LABEL_SCALE_VOLTAGE;
        default:                break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // name

    /**
     * @brief Returns the name.
     *
     * @return Pointer to the name; `nullptr` when no value is available.
     */
    const char *name() const { return _name; }
    /**
     * @brief Sets the name.
     *
     * @param[in] name Null-terminated name or label consumed by the operation.
     */
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }

    // mode

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const { return _mode; }
    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _mode) {
            _mode = mode;
            clearItems();
        }
    }

    /**
     * @brief Adjusts the mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMode(int value, bool shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    /**
     * @brief Formats the mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
    }

    // size

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    int size() const { return _size; }
    /**
     * @brief Sets the size.
     *
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void setSize(int size) {
        _size = clamp(size, _mode == Mode::Chromatic ? 1 : 2, CONFIG_USER_SCALE_SIZE);
    }

    /**
     * @brief Adjusts the size from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editSize(int value, bool shift) {
        setSize(size() + value);
    }

    /**
     * @brief Formats the size into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printSize(StringBuilder &str) const {
        str(TXT_MODEL_GENERIC_VALUE, size());
    }

    // items

    /**
     * @brief Returns the items.
     *
     * @return Reference to the stored item collection.
     */
    const ItemArray &items() const { return _items; }
          /**
           * @brief Returns the items.
           *
           * @return Reference to the stored item collection.
           */
          ItemArray &items()       { return _items; }

    /**
     * @brief Returns item.
     *
     * @param[in] index Zero-based item index.
     *
     * @return Result of item().
     */
    int item(int index) const { return _items[index]; }
    /**
     * @brief Sets the item.
     *
     * @param[in] index Zero-based item index.
     * @param[in] value New item to store or apply.
     */
    void setItem(int index, int value) {
        switch (_mode) {
        case Mode::Chromatic:
            _items[index] = clamp(value, 0, 11);
            break;
        case Mode::Voltage:
            _items[index] = clamp(value, -5000, 5000);
            break;
        case Mode::Last:
            break;
        }
    }

    /**
     * @brief Adjusts the item from a UI edit delta.
     *
     * @param[in] index Zero-based item index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editItem(int index, int value, int shift) {
        switch (_mode) {
        case Mode::Chromatic:
            setItem(index, item(index) + value);
            break;
        case Mode::Voltage:
            setItem(index, item(index) + value * (shift ? 100 : 1));
            break;
        case Mode::Last:
            break;
        }
    }

    /**
     * @brief Formats the item into the supplied string builder/output.
     *
     * @param[in] index Zero-based item index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void printItem(int index, StringBuilder &str) const {
        switch (_mode) {
        case Mode::Chromatic:
            noteNameChromaticMode(str, index, 0, Scale::Short1);
            break;
        case Mode::Voltage:
            str(TXT_MODEL_VOLTAGE, _items[index] * (1.f / 1000.f));
            break;
        case Mode::Last:
            break;
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a UserScale instance.
     */
    UserScale();

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();
    /**
     * @brief Clears items.
     */
    void clearItems();

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     *
     * @return `true` if read; otherwise `false`.
     */
    bool read(VersionedSerializedReader &reader);

    //----------------------------------------
    // Scale implementation
    //----------------------------------------

    /**
     * @brief Reports whether chromatic.
     *
     * @return `true` if chromatic; otherwise `false`.
     */
    bool isChromatic() const override {
        return mode() == Mode::Chromatic;
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
        switch (_mode) {
        case Mode::Chromatic:
            noteNameChromaticMode(str, note, rootNote, format);
            break;
        case Mode::Voltage:
            noteNameVoltageMode(str, note, format);
            break;
        case Mode::Last:
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
        int notesPerOctave_ = notesPerOctave();
        int octave = roundDownDivide(note, notesPerOctave_);
        int index = note - octave * notesPerOctave_;
        switch (_mode) {
        case Mode::Chromatic:
            return octave + _items[index] * (1.f / 12.f);
        case Mode::Voltage:
            return octave * octaveRangeVolts() + _items[index] * (1.f / 1000.f);
        case Mode::Last:
            break;
        }
        return 0.f;
    }

    /**
     * @brief Returns note from volts.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Note value derived/quantized from the supplied voltage.
     */
    int noteFromVolts(float volts) const override {
        switch (_mode) {
        case Mode::Chromatic:
            return noteFromVoltsChromaticMode(volts);
        case Mode::Voltage:
            return noteFromVoltsVoltageMode(volts);
        case Mode::Last:
            break;
        }
        return 0;
    }

    /**
     * @brief Returns the notes per octave.
     *
     * @return Number of scale degrees that make up one octave of the scale.
     */
    int notesPerOctave() const override {
#ifdef FIX_BROKEN_SCALES
        if (_mode == Mode::Chromatic) {
            return std::max(1, int(_size));
        }
        return std::max(1, int(_size) - 1);
#else
        return _mode == Mode::Chromatic ? _size : _size - 1;
#endif
    }

    static Array userScales; ///< Owned fixed-size collection of user scales.

private:
    /**
     * @brief Returns a note name using chromatic naming rules.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] rootNote Root note used by the scale/note operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    void noteNameChromaticMode(StringBuilder &str, int note, int rootNote, Format format) const {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        bool printNote = format == Short1 || format == Long;
        bool printOctave = format == Short2 || format == Long;

        int octave = roundDownDivide(note, _size);

        int noteIndex = _items[note - octave * _size] + rootNote;
        while (noteIndex >= 12) {
            noteIndex -= 12;
            octave += 1;
        }

        if (printNote) {
            Types::printNote(str, noteIndex);
        }

        if (printOctave) {
            str(TXT_MODEL_OCTAVE, octave);
        }
    }

    /**
     * @brief Returns a note name using voltage-oriented naming rules.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    void noteNameVoltageMode(StringBuilder &str, int note, Format format) const {
#ifdef FIX_BROKEN_SCALES
        note = clampNote(note);
#endif
        float volts = noteToVolts(note);
        switch (format) {
        case Short1:
            str(TXT_MODEL_GENERIC_CHAR, volts < 0.f ? '-' : '+');
            break;
        case Short2:
            str(TXT_MODEL_NOTE_OCTAVE, std::abs(volts));
            break;
        case Long:
            str(TXT_MODEL_VOLTAGE, volts);
            break;
        }
    }

    /**
     * @brief Returns note from volts chromatic mode.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Result of noteFromVoltsChromaticMode().
     */
    int noteFromVoltsChromaticMode(float volts) const {
        int semiNotes = std::floor(volts * 12.f + 0.01f);
        int octave = roundDownDivide(semiNotes, 12);
        semiNotes -= octave * 12;

        int index = -1;
        for (int i = 0; i < _size; ++i) {
            if (semiNotes < _items[i]) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _size -1;
            --octave;
        }
#ifdef FIX_BROKEN_SCALES
        return clampNote(octave * _size + index);
#else
        return octave * _size + index;
#endif
    }

    /**
     * @brief Returns note from volts voltage mode.
     *
     * @param[in] volts Voltage value in volts.
     *
     * @return Result of noteFromVoltsVoltageMode().
     */
    int noteFromVoltsVoltageMode(float volts) const {
        float octaveRange = octaveRangeVolts();
        int octave = int(std::floor(volts / octaveRange));
        volts -= octave * octaveRange;
        int itemValue = int(std::floor(volts * 1000.f));

        int index = -1;
        for (int i = 0; i < _size; ++i) {
            if (itemValue < _items[i]) {
                break;
            }
            index = i;
        }

        if (index == -1) {
            index = _size -1;
            --octave;
        }

#ifdef FIX_BROKEN_SCALES
        return clampNote(octave * (_size - 1) + index);
#else
        return octave * (_size - 1) + index;
#endif
    }

#ifdef FIX_BROKEN_SCALES
    /**
     * @brief Returns the octave range volts.
     *
     * @return Voltage span corresponding to one complete octave of the user scale.
     */
    float octaveRangeVolts() const {
        if (_size < 2) {
            return 1.f;
        }

        float range = (_items[_size - 1] - _items[0]) * (1.f / 1000.f);
        return std::max(0.001f, range);
    }
#else
    /**
     * @brief Returns the octave range volts.
     *
     * @return Voltage span corresponding to one complete octave of the user scale.
     */
    float octaveRangeVolts() const {
        return (_items[_size - 1] - _items[0]) * (1.f / 1000.f);
    }
#endif

    char _name[NameLength + 1]; ///< Null-terminated user-scale name with storage for `NameLength` characters plus the terminator.
    Mode _mode; ///< Active mode controlling the behavior of `UserScale`.
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    uint8_t _size; ///< Size of the associated data in bytes/elements as defined by this type.
    /**
     * @brief Stored user-scale entries in scale order.
     */
    ItemArray _items; ///< Stored calibration/scale item values indexed by the model-defined item domain.
};
