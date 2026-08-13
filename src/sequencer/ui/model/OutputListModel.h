/**
 * @file OutputListModel.h
 * @brief Declares the OutputListModel component used by the sequencer UI.
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

#include "model/MidiOutput.h"

/**
 * @brief Provides list data and editing behavior for output.
 */
class OutputListModel : public ListModel {
public:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Target, ///< Selects the target item.
        Event, ///< Selects the event item.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Enumerates the supported note item values.
     */
    enum NoteItem {
        GateSource = Last, ///< Selects the gate source note item.
        NoteSource, ///< Selects the note source note item.
        VelocitySource, ///< Selects the velocity source note item.
        LastNoteItem, ///< Selects the last note item note item.
    };

    /**
     * @brief Enumerates the supported control change item values.
     */
    enum ControlChangeItem {
        ControlNumber = Last, ///< Selects the control number control change item.
        ControlSource, ///< Selects the control source control change item.
        LastControlChangeItem, ///< Selects the last control change item control change item.
    };

    /**
     * @brief Constructs a OutputListModel instance.
     *
     * @param[out] output Output descriptor populated with the selected list/model entry.
     */
    OutputListModel(MidiOutput::Output &output) :
        /**
         * @brief Returns the output.
         */
        _output(output)
    {}

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        switch (_output.event()) {
        case MidiOutput::Output::Event::None:
            return Last;
        case MidiOutput::Output::Event::Note:
            return LastNoteItem;
        case MidiOutput::Output::Event::ControlChange:
            return LastControlChangeItem;
        default:
            return Last;
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
        case Event:         return TXT_LIST_LABEL_EVENT;
        case Last:          break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:    return TXT_LIST_LABEL_GATE_SOURCE;
            case NoteSource:    return TXT_LIST_LABEL_NOTE_SOURCE;
            case VelocitySource:return TXT_LIST_LABEL_VELOCITY_SOURCE;
            case LastNoteItem:  break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber: return TXT_LIST_LABEL_CONTROL_NUMBER;
            case ControlSource: return TXT_LIST_LABEL_CONTROL_SOURCE;
            case LastControlChangeItem: break;
            }
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
            _output.target().print(str);
            break;
        case Event:
            _output.printEvent(str);
            break;
        case Last:
            break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:
                _output.printGateSource(str);
                break;
            case NoteSource:
                _output.printNoteSource(str);
                break;
            case VelocitySource:
                _output.printVelocitySource(str);
                break;
            case LastNoteItem:
            break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber:
                _output.printControlNumber(str);
                break;
            case ControlSource:
                _output.printControlSource(str);
                break;
            case LastControlChangeItem:
                break;
            }
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
            _output.target().edit(value, shift);
            break;
        case Event:
            _output.editEvent(value, shift);
            break;
        case Last:
            break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:
                _output.editGateSource(value, shift);
                break;
            case NoteSource:
                _output.editNoteSource(value, shift);
                break;
            case VelocitySource:
                _output.editVelocitySource(value, shift);
                break;
            case LastNoteItem:
            break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber:
                _output.editControlNumber(value, shift);
                break;
            case ControlSource:
                _output.editControlSource(value, shift);
                break;
            case LastControlChangeItem:
                break;
            }
        }
    }

    /**
     * @brief Reference to output owned by another component.
     */
    MidiOutput::Output &_output; ///< Reference to output owned by another component.
};
