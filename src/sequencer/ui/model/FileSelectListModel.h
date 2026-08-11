/**
 * @file FileSelectListModel.h
 * @brief Declares the FileSelectListModel component used by the sequencer UI.
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

#include "model/FileManager.h"

class FileSelectListModel : public ListModel {
public:
    void setType(FileType type) {
        _type = type;
    }

    virtual int rows() const override {
        return 128;
    }

    virtual int columns() const override {
        return 1;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(row, str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
    }

private:
    void formatName(int row, StringBuilder &str) const {
        FileManager::SlotInfo info;
        FileManager::slotInfo(_type, row, info);
        str(TXT_LIST_LABEL_FILE, row + 1, info.used ? info.name : TXT_LIST_LABEL_EMPTY);
    }

    FileType _type;
};
