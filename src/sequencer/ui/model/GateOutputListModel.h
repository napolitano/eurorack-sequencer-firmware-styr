/**
 * @file GateOutputListModel.h
 * @brief Declares the GateOutputListModel component used by the sequencer UI.
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

#include "model/Project.h"

#include <array>

/**
 * @brief Provides list data and editing behavior for gate output.
 */
class GateOutputListModel : public ListModel {
public:
    /**
     * @brief Constructs a GateOutputListModel instance.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    GateOutputListModel(Project &project) :
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
        return CONFIG_TRACK_COUNT;
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
            str(TXT_LIST_LABEL_GATE_VALUE, row + 1);
        } else if (column == 1) {
            int trackIndex = _project.gateOutputTrack(row);
            int outputIndex = 0;
            for (int i = 0; i < row; ++i) {
                outputIndex += _project.gateOutputTrack(i) == trackIndex ? 1 : 0;
            }
            str(TXT_LIST_LABEL_TRACK, trackIndex + 1);
            _project.track(trackIndex).gateOutputName(outputIndex, str);
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
            _project.editGateOutputTrack(row, value, shift);
        }
    }

private:
    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
};
