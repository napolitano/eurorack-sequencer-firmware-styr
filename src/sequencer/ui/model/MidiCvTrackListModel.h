/**
 * @file MidiCvTrackListModel.h
 * @brief Declares the MidiCvTrackListModel component used by the sequencer UI.
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

#include "RoutableListModel.h"

#include "model/MidiCvTrack.h"

/**
 * @brief Provides list data and editing behavior for midi cv track.
 */
class MidiCvTrackListModel : public RoutableListModel {
public:
    /**
     * @brief Sets the track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void setTrack(MidiCvTrack &track) {
        _track = &track;
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return Last;
    }

    /**
     * @brief Returns the columns.
     *
     * @return Number of columns represented by this object.
     */
    virtual int columns() const override {
        return 2;
    }

    /**
     * @brief Returns the cell at the requested row and column.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[out] str String builder that receives the formatted representation.
     */
    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

    /**
     * @brief Returns routing target.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Resolved routing target identifier.
     */
    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case SlideTime:
            return Routing::Target::SlideTime;
        case Transpose:
            return Routing::Target::Transpose;
        default:
            return Routing::Target::None;
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Source, ///< Selects the source item.
        Voices, ///< Selects the voices item.
        VoiceConfig, ///< Selects the voice config item.
        NotePriority, ///< Selects the note priority item.
        LowNote, ///< Selects the low note item.
        HighNote, ///< Selects the high note item.
        PitchBendRange, ///< Selects the pitch bend range item.
        ModulationRange, ///< Selects the modulation range item.
        Retrigger, ///< Selects the retrigger item.
        SlideTime, ///< Selects the slide time item.
        Transpose, ///< Selects the transpose item.
        ArpeggiatorEnabled, ///< Selects the arpeggiator enabled item.
        ArpeggiatorHold, ///< Selects the arpeggiator hold item.
        ArpeggiatorMode, ///< Selects the arpeggiator mode item.
        ArpeggiatorDivisor, ///< Selects the arpeggiator divisor item.
        ArpeggiatorGateLength, ///< Selects the arpeggiator gate length item.
        ArpeggiatorOctaves, ///< Selects the arpeggiator octaves item.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for item.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Pointer to the item name; `nullptr` when no value is available.
     */
    static const char *itemName(Item item) {
        switch (item) {
        case Source:                return TXT_LIST_LABEL_SOURCE;
        case Voices:                return TXT_LIST_LABEL_VOICES;
        case VoiceConfig:           return TXT_LIST_LABEL_VOICE_CONFIG;
        case NotePriority:          return TXT_LIST_LABEL_NOTE_PRIORITY;
        case LowNote:               return TXT_LIST_LABEL_LOW_NOTE;
        case HighNote:              return TXT_LIST_LABEL_HIGH_NOTE;
        case PitchBendRange:        return TXT_LIST_LABEL_PITCH_BEND;
        case ModulationRange:       return TXT_LIST_LABEL_MOD_RANGE;
        case Retrigger:             return TXT_LIST_LABEL_RETRIGGER;
        case SlideTime:             return TXT_LIST_LABEL_SLIDE_TIME;
        case Transpose:             return TXT_LIST_LABEL_TRANSPOSE;
        case ArpeggiatorEnabled:    return TXT_LIST_LABEL_ARPEGGIATOR;
        case ArpeggiatorHold:       return TXT_LIST_LABEL_HOLD;
        case ArpeggiatorMode:       return TXT_LIST_LABEL_MODE;
        case ArpeggiatorDivisor:    return TXT_LIST_LABEL_DIVISOR;
        case ArpeggiatorGateLength: return TXT_LIST_LABEL_GATE_LENGTH;
        case ArpeggiatorOctaves:    return TXT_LIST_LABEL_OCTAVE_PLURAL;
        case Last:                  break;
        }
        return nullptr;
    }

    /**
     * @brief Formats the name for display.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[out] str String builder that receives the formatted representation.
     */
    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    /**
     * @brief Formats the value for display.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[out] str String builder that receives the formatted representation.
     */
    void formatValue(Item item, StringBuilder &str) const {
        const auto &arpeggiator = _track->arpeggiator();

        switch (item) {
        case Source:
            _track->source().print(str);
            break;
        case Voices:
            _track->printVoices(str);
            break;
        case VoiceConfig:
            _track->printVoiceConfig(str);
            break;
        case NotePriority:
            _track->printNotePriority(str);
            break;
        case LowNote:
            _track->printLowNote(str);
            break;
        case HighNote:
            _track->printHighNote(str);
            break;
        case PitchBendRange:
            _track->printPitchBendRange(str);
            break;
        case ModulationRange:
            _track->printModulationRange(str);
            break;
        case Retrigger:
            _track->printRetrigger(str);
            break;
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Transpose:
            _track->printTranspose(str);
            break;
        case ArpeggiatorEnabled:
            arpeggiator.printEnabled(str);
            break;
        case ArpeggiatorHold:
            arpeggiator.printHold(str);
            break;
        case ArpeggiatorMode:
            arpeggiator.printMode(str);
            break;
        case ArpeggiatorDivisor:
            arpeggiator.printDivisor(str);
            break;
        case ArpeggiatorGateLength:
            arpeggiator.printGateLength(str);
            break;
        case ArpeggiatorOctaves:
            arpeggiator.printOctaves(str);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Adjusts the value from a UI edit delta.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editValue(Item item, int value, bool shift) {
        auto &arpeggiator = _track->arpeggiator();

        switch (item) {
        case Source:
            _track->source().edit(value, shift);
            break;
        case Voices:
            _track->editVoices(value, shift);
            break;
        case VoiceConfig:
            _track->editVoiceConfig(value, shift);
            break;
        case NotePriority:
            _track->editNotePriority(value, shift);
            break;
        case LowNote:
            _track->editLowNote(value, shift);
            break;
        case HighNote:
            _track->editHighNote(value, shift);
            break;
        case PitchBendRange:
            _track->editPitchBendRange(value, shift);
            break;
        case ModulationRange:
            _track->editModulationRange(value, shift);
            break;
        case Retrigger:
            _track->editRetrigger(value, shift);
            break;
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case ArpeggiatorEnabled:
            arpeggiator.editEnabled(value, shift);
            break;
        case ArpeggiatorHold:
            arpeggiator.editHold(value, shift);
            break;
        case ArpeggiatorMode:
            arpeggiator.editMode(value, shift);
            break;
        case ArpeggiatorDivisor:
            arpeggiator.editDivisor(value, shift);
            break;
        case ArpeggiatorGateLength:
            arpeggiator.editGateLength(value, shift);
            break;
        case ArpeggiatorOctaves:
            arpeggiator.editOctaves(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Persistent track model operated on by this component.
     */
    MidiCvTrack *_track; ///< Persistent track model operated on by this component.
};
