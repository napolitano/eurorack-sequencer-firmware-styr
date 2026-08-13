/**
 * @file ListModel.h
 * @brief Declares the ListModel component used by the sequencer UI.
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

#include "core/utils/StringBuilder.h"

/**
 * @brief Provides list data and editing behavior for .
 */
class ListModel {
public:
    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const = 0;
    /**
     * @brief Returns the columns.
     *
     * @return Number of columns represented by this object.
     */
    virtual int columns() const = 0;

    /**
     * @brief Returns the cell at the requested row and column.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[out] str String builder that receives the formatted representation.
     */
    virtual void cell(int row, int column, StringBuilder &str) const = 0;

    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    virtual void edit(int row, int column, int value, bool shift) = 0;

    /**
     * @brief Returns indexed count.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Number of indexed entries.
     */
    virtual int indexedCount(int row) const { return 0; }
    /**
     * @brief Returns indexed.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Indexed state/value.
     */
    virtual int indexed(int row) const { return -1; }
    /**
     * @brief Sets the indexed.
     *
     * @param[in] row Zero-based row index.
     * @param[in] index Zero-based indexed index.
     */
    virtual void setIndexed(int row, int index) {}
};
