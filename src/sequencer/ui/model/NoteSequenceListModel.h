/**
 * @file NoteSequenceListModel.h
 * @brief Declares the NoteSequenceListModel component used by the sequencer UI.
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

#include "model/NoteSequence.h"
#include "model/Scale.h"

/**
 * @brief Provides list data and editing behavior for note sequence.
 */
class NoteSequenceListModel : public RoutableListModel {
public:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        FirstStep, ///< Selects the first step item.
        LastStep, ///< Selects the last step item.
        RunMode, ///< Selects the run mode item.
        Divisor, ///< Selects the divisor item.
        ResetMeasure, ///< Selects the reset measure item.
        Scale, ///< Selects the scale item.
        RootNote, ///< Selects the root note item.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Constructs a NoteSequenceListModel instance.
     */
    NoteSequenceListModel()
    {}

    /**
     * @brief Sets the sequence.
     *
     * @param[in] sequence Sequence to inspect or modify.
     */
    void setSequence(NoteSequence *sequence) {
        _sequence = sequence;
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return _sequence ? Last : 0;
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
     * @brief Returns indexed count.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Number of indexed entries.
     */
    virtual int indexedCount(int row) const override {
        return indexedCountValue(Item(row));
    }

    /**
     * @brief Returns indexed.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Indexed state/value.
     */
    virtual int indexed(int row) const override {
        return indexedValue(Item(row));
    }

    /**
     * @brief Sets the indexed.
     *
     * @param[in] row Zero-based row index.
     * @param[in] index Zero-based indexed index.
     */
    virtual void setIndexed(int row, int index) override {
        if (index >= 0 && index < indexedCount(row)) {
            setIndexedValue(Item(row), index);
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
        case Divisor:
            return Routing::Target::Divisor;
        case FirstStep:
            return Routing::Target::FirstStep;
        case LastStep:
            return Routing::Target::LastStep;
        case RunMode:
            return Routing::Target::RunMode;
        case Scale:
            return Routing::Target::Scale;
        case RootNote:
            return Routing::Target::RootNote;
        default:
            return Routing::Target::None;
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
    static const char *itemName(Item item) {
        switch (item) {
        case FirstStep:         return TXT_LIST_LABEL_FIRST_STEP;
        case LastStep:          return TXT_LIST_LABEL_LAST_STEP;
        case RunMode:           return TXT_LIST_LABEL_RUN_MODE;
        case Divisor:           return TXT_LIST_LABEL_DIVISOR;
        case ResetMeasure:      return TXT_LIST_LABEL_RESET_MEASURE;
        case Scale:             return TXT_LIST_LABEL_SCALE;
        case RootNote:          return TXT_LIST_LABEL_ROOT_NOTE;
        case Last:              break;
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
        case FirstStep:
            _sequence->printFirstStep(str);
            break;
        case LastStep:
            _sequence->printLastStep(str);
            break;
        case RunMode:
            _sequence->printRunMode(str);
            break;
        case Divisor:
            _sequence->printDivisor(str);
            break;
        case ResetMeasure:
            _sequence->printResetMeasure(str);
            break;
        case Scale:
            _sequence->printScale(str);
            break;
        case RootNote:
            _sequence->printRootNote(str);
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
        case FirstStep:
            _sequence->editFirstStep(value, shift);
            break;
        case LastStep:
            _sequence->editLastStep(value, shift);
            break;
        case RunMode:
            _sequence->editRunMode(value, shift);
            break;
        case Divisor:
            _sequence->editDivisor(value, shift);
            break;
        case ResetMeasure:
            _sequence->editResetMeasure(value, shift);
            break;
        case Scale:
            _sequence->editScale(value, shift);
            break;
        case RootNote:
            _sequence->editRootNote(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Returns indexed count value.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Result of indexedCountValue().
     */
    int indexedCountValue(Item item) const {
        switch (item) {
        case FirstStep:
        case LastStep:
            return 16;
        case RunMode:
            return int(Types::RunMode::Last);
        case Divisor:
        case ResetMeasure:
            return 16;
        case Scale:
            return Scale::Count + 1;
        case RootNote:
            return 12 + 1;
        case Last:
            break;
        }
        return -1;
    }

    /**
     * @brief Returns indexed value.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Result of indexedValue().
     */
    int indexedValue(Item item) const {
        switch (item) {
        case FirstStep:
            return _sequence->firstStep();
        case LastStep:
            return _sequence->lastStep();
        case RunMode:
            return int(_sequence->runMode());
        case Divisor:
            return _sequence->indexedDivisor();
        case ResetMeasure:
            return _sequence->resetMeasure();
        case Scale:
            return _sequence->indexedScale();
        case RootNote:
            return _sequence->indexedRootNote();
        case Last:
            break;
        }
        return -1;
    }

    /**
     * @brief Sets the indexed value.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[in] index Zero-based indexed index.
     */
    void setIndexedValue(Item item, int index) {
        switch (item) {
        case FirstStep:
            return _sequence->setFirstStep(index);
        case LastStep:
            return _sequence->setLastStep(index);
        case RunMode:
            return _sequence->setRunMode(Types::RunMode(index));
        case Divisor:
            return _sequence->setIndexedDivisor(index);
        case ResetMeasure:
            return _sequence->setResetMeasure(index);
        case Scale:
            return _sequence->setIndexedScale(index);
        case RootNote:
            return _sequence->setIndexedRootNote(index);
        case Last:
            break;
        }
    }

    /**
     * @brief Currently active sequence.
     */
    NoteSequence *_sequence; ///< Currently active sequence.
};
