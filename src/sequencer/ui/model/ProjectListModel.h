/**
 * @file ProjectListModel.h
 * @brief Declares the ProjectListModel component used by the sequencer UI.
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

#include "model/Project.h"

/**
 * @brief Provides list data and editing behavior for project.
 */
class ProjectListModel : public RoutableListModel {
public:
    /**
     * @brief Constructs a ProjectListModel instance.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    ProjectListModel(Project &project) :
        /**
         * @brief Returns the project.
         */
        _project(project)
    {}

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
        case Tempo:
            return Routing::Target::Tempo;
        case Swing:
            return Routing::Target::Swing;
        default:
            return Routing::Target::None;
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Name, ///< Selects the name item.
        Tempo, ///< Selects the tempo item.
        Swing, ///< Selects the swing item.
        TimeSignature, ///< Selects the time signature item.
        SyncMeasure, ///< Selects the sync measure item.
        Scale, ///< Selects the scale item.
        RootNote, ///< Selects the root note item.
        MonitorMode, ///< Selects the monitor mode item.
        RecordMode, ///< Selects the record mode item.
        MidiInput, ///< Selects the midi input item.
        CvGateInput, ///< Selects the cv gate input item.
        CurveCvInput, ///< Selects the curve cv input item.
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
        case Name:              return TXT_LIST_LABEL_NAME;
        case Tempo:             return TXT_LIST_LABEL_TEMPO;
        case Swing:             return TXT_LIST_LABEL_SWING;
        case TimeSignature:     return TXT_LIST_LABEL_TIME_SIGNATURE;
        case SyncMeasure:       return TXT_LIST_LABEL_SYNC_MEASURE;
        case Scale:             return TXT_LIST_LABEL_SCALE;
        case RootNote:          return TXT_LIST_LABEL_ROOT_NOTE;
        case MonitorMode:       return TXT_LIST_LABEL_MONITOR_MODE;
        case RecordMode:        return TXT_LIST_LABEL_RECORD_MODE;
        case MidiInput:         return TXT_LIST_LABEL_MIDI_INPUT;
        case CvGateInput:       return TXT_LIST_LABEL_CVGATE_INPUT;
        case CurveCvInput:      return TXT_LIST_LABEL_CURVECV_INPUT;
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
        case Name:
            str(_project.name());
            break;
        case Tempo:
            _project.printTempo(str);
            break;
        case Swing:
            _project.printSwing(str);
            break;
        case TimeSignature:
            _project.printTimeSignature(str);
            break;
        case SyncMeasure:
            _project.printSyncMeasure(str);
            break;
        case Scale:
            _project.printScale(str);
            break;
        case RootNote:
            _project.printRootNote(str);
            break;
        case MonitorMode:
            _project.printMonitorMode(str);
            break;
        case RecordMode:
            _project.printRecordMode(str);
            break;
        case MidiInput:
            _project.printMidiInput(str);
            break;
        case CvGateInput:
            _project.printCvGateInput(str);
            break;
        case CurveCvInput:
            _project.printCurveCvInput(str);
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
        case Name:
            break;
        case Tempo:
            _project.editTempo(value, shift);
            break;
        case Swing:
            _project.editSwing(value, shift);
            break;
        case TimeSignature:
            _project.editTimeSignature(value, shift);
            break;
        case SyncMeasure:
            _project.editSyncMeasure(value, shift);
            break;
        case Scale:
            _project.editScale(value, shift);
            break;
        case RootNote:
            _project.editRootNote(value, shift);
            break;
        case MonitorMode:
            _project.editMonitorMode(value, shift);
            break;
        case RecordMode:
            _project.editRecordMode(value, shift);
            break;
        case MidiInput:
            _project.editMidiInput(value, shift);
            break;
        case CvGateInput:
            _project.editCvGateInput(value, shift);
            break;
        case CurveCvInput:
            _project.editCurveCvInput(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
};
