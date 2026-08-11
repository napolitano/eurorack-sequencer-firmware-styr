/**
 * @file LinkTrackListModel.h
 * @brief Declares the LinkTrackListModel component used by the sequencer UI.
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

class LinkTrackListModel : public ListModel {
public:
    LinkTrackListModel(Project &project) :
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
            str(TXT_LIST_LABEL_TRACK, row + 1);
        } else if (column == 1) {
            _project.track(row).printLinkTrack(str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _project.track(row).editLinkTrack(value, shift);
        }
    }

private:
    Project &_project;
};
