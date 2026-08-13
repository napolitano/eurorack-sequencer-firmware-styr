/**
 * @file RouteListModel.h
 * @brief Declares the RouteListModel component used by the sequencer UI.
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

#include "ListModel.h"

#include "model/Routing.h"

/**
 * @brief Provides list data and editing behavior for route.
 */
class RouteListModel : public ListModel {
public:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Target, ///< Selects the target item.
        Min, ///< Selects the min item.
        Max, ///< Selects the max item.
        Tracks, ///< Selects the tracks item.
        Source, ///< Selects the source item.
        FirstSource, ///< Selects the first source item.
        CvRange = FirstSource, ///< Selects the cv range item.
        MidiSource = FirstSource, ///< Selects the midi source item.
        MidiEvent, ///< Selects the midi event item.
        FirstMidiEventConfig, ///< Selects the first midi event config item.
        MidiControlNumber = FirstMidiEventConfig, ///< Selects the midi control number item.
        MidiNote = FirstMidiEventConfig, ///< Selects the midi note item.
        MidiNoteRange, ///< Selects the midi note range item.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Constructs a RouteListModel instance.
     *
     * @param[in] route Routing entry displayed or edited by the list model.
     */
    RouteListModel(Routing::Route &route) :
        /**
         * @brief Returns the route.
         */
        _route(route)
    {}

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        bool isEmpty = _route.target() == Routing::Target::None;
        bool isCvSource = Routing::isCvSource(_route.source());
        bool isMidiSource = Routing::isMidiSource(_route.source());
        bool hasNoteOrController = _route.midiSource().event() != Routing::MidiSource::Event::PitchBend;
        bool hasNoteRange = _route.midiSource().event() == Routing::MidiSource::Event::NoteRange;
        if (isEmpty) {
            return 1;
        } else if (isCvSource) {
            return FirstSource + 1;
        } else if (isMidiSource) {
            return hasNoteOrController ? (hasNoteRange ? Last : int(Last) - 1) : int(Last) - 2;
        } else {
            return FirstSource;
        }
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

private:
    /**
     * @brief Returns the display name for item.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Pointer to the item name; `nullptr` when no value is available.
     */
    const char *itemName(Item item) const {
        switch (item) {
        case Target:        return TXT_LIST_LABEL_TARGET;
        case Min:           return TXT_LIST_LABEL_MINIMUM;
        case Max:           return TXT_LIST_LABEL_MAXIMUM;
        case Tracks:        return TXT_LIST_LABEL_TRACKS;
        case Source:        return TXT_LIST_LABEL_SOURCE;
        // case CvRange:
        case MidiSource:    return Routing::isCvSource(_route.source()) ? TXT_LIST_LABEL_RANGE : TXT_LIST_LABEL_MIDI_SOURCE;
        case MidiEvent:     return TXT_LIST_LABEL_MIDI_EVENT;
        // case MidiControlNumber:
        case MidiNote:
                            return _route.midiSource().isControlEvent() ? TXT_LIST_LABEL_CC_NUMBER : TXT_LIST_LABEL_NOTE;
        case MidiNoteRange: return TXT_LIST_LABEL_NOTE_RANGE;
        case Last:          break;
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
        switch (item) {
        case Target:
            _route.printTarget(str);
            break;
        case Min:
            _route.printMin(str);
            break;
        case Max:
            _route.printMax(str);
            break;
        case Tracks:
            _route.printTracks(str);
            break;
        case Source:
            _route.printSource(str);
            break;
        // case CvRange:
        case MidiSource:
            if (Routing::isCvSource(_route.source())) {
                _route.cvSource().printRange(str);
            } else {
                _route.midiSource().source().print(str);
            }
            break;
        case MidiEvent:
            _route.midiSource().printEvent(str);
            break;
        // case MidiControlNumber:
        case MidiNote:
            if (_route.midiSource().isControlEvent()) {
                _route.midiSource().printControlNumber(str);
            } else {
                _route.midiSource().printNote(str);
            }
            break;
        case MidiNoteRange:
            _route.midiSource().printNoteRange(str);
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
        switch (item) {
        case Target:
            _route.editTarget(value, shift);
            break;
        case Min:
            _route.editMin(value, shift);
            break;
        case Max:
            _route.editMax(value, shift);
            break;
        case Tracks:
            // handled in RoutePage
            break;
        case Source:
            _route.editSource(value, shift);
            break;
        // case CvRange:
        case MidiSource:
            if (Routing::isCvSource(_route.source())) {
                _route.cvSource().editRange(value, shift);
            } else {
                _route.midiSource().source().edit(value, shift);
            }
            break;
        case MidiEvent:
            _route.midiSource().editEvent(value, shift);
            break;
        // case MidiControlNumber:
        case MidiNote:
            if (_route.midiSource().isControlEvent()) {
                _route.midiSource().editControlNumber(value, shift);
            } else {
                _route.midiSource().editNote(value, shift);
            }
            break;
        case MidiNoteRange:
            _route.midiSource().editNoteRange(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Reference to route owned by another component.
     */
    Routing::Route &_route; ///< Reference to route owned by another component.
};
