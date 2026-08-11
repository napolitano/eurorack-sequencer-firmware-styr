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

class CalibrationCvOutputListModel : public ListModel {
public:
    CalibrationCvOutputListModel()
    {}

    void setCvOutput(Calibration::CvOutput &cvOutput) {
        _cvOutput = &cvOutput;
    }

    virtual int rows() const override {
        return Calibration::CvOutput::ItemCount;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            Calibration::CvOutput::itemName(str, row);
        } else if (column == 1) {
            _cvOutput->printItem(row, str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _cvOutput->editItem(row, value, shift);
        }
    }

private:
    Calibration::CvOutput *_cvOutput;
};
