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

class GateOutputListModel : public ListModel {
public:
    GateOutputListModel(Project &project) :
        _project(project)
    {}

    virtual int rows() const override {
        return CONFIG_TRACK_COUNT;
    }

    virtual int columns() const override {
        return 2;
    }

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

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _project.editGateOutputTrack(row, value, shift);
        }
    }

private:
    Project &_project;
};
