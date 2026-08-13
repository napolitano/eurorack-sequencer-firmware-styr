/**
 * @file CalibrationCvOutputListModel.h
 * @brief Declares the CalibrationCvOutputListModel component used by the sequencer UI.
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

#include "model/Calibration.h"

/**
 * @brief Provides list data and editing behavior for calibration cv output.
 */
class CalibrationCvOutputListModel : public ListModel {
public:
    /**
     * @brief Constructs a CalibrationCvOutputListModel instance.
     */
    CalibrationCvOutputListModel()
    {}

    /**
     * @brief Sets the cv output.
     *
     * @param[in] cvOutput Cv output consumed by `setCvOutput()`.
     */
    void setCvOutput(Calibration::CvOutput &cvOutput) {
        _cvOutput = &cvOutput;
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return Calibration::CvOutput::ItemCount;
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
            Calibration::CvOutput::itemName(str, row);
        } else if (column == 1) {
            _cvOutput->printItem(row, str);
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
            _cvOutput->editItem(row, value, shift);
        }
    }

private:
    /**
     * @brief Current control-voltage output value in the engine output domain.
     */
    Calibration::CvOutput *_cvOutput; ///< Current control-voltage output value in the engine output domain.
};
