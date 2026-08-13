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

/**
 * @brief Provides list data and editing behavior for file select.
 */
class FileSelectListModel : public ListModel {
public:
    /**
     * @brief Sets the type.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     */
    void setType(FileType type) {
        _type = type;
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return 128;
    }

    /**
     * @brief Returns the columns.
     *
     * @return Number of columns represented by this object.
     */
    virtual int columns() const override {
        return 1;
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
            formatName(row, str);
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
    }

private:
    /**
     * @brief Formats the name for display.
     *
     * @param[in] row Zero-based row index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void formatName(int row, StringBuilder &str) const {
        FileManager::SlotInfo info;
        FileManager::slotInfo(_type, row, info);
        str(TXT_LIST_LABEL_FILE, row + 1, info.used ? info.name : TXT_LIST_LABEL_EMPTY);
    }

    FileType _type; ///< File type associated with the storage operation.
};
