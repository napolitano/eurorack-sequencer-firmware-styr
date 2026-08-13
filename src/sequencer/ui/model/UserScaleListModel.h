/**
 * @file UserScaleListModel.h
 * @brief Declares the UserScaleListModel component used by the sequencer UI.
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

#include "model/UserScale.h"

/**
 * @brief Provides list data and editing behavior for user scale.
 */
class UserScaleListModel : public ListModel {
public:
    /**
     * @brief Constructs a UserScaleListModel instance.
     */
    UserScaleListModel()
    {}

    /**
     * @brief Sets the user scale.
     *
     * @param[in] userScale User-defined scale read or modified by the operation.
     */
    void setUserScale(UserScale &userScale) {
        _userScale = &userScale;
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return Item::Last + _userScale->size();
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
        if (row < int(Item::Last)) {
            if (column == 0) {
                formatName(Item(row), str);
            } else if (column == 1) {
                formatValue(Item(row), str);
            }
        } else {
            int index = row - int(Item::Last);
            if (column == 0) {
                str(TXT_LIST_LABEL_ITEM, index + 1);
            } else if (column == 1) {
                _userScale->printItem(index, str);
            }
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
        if (row < int(Item::Last)) {
            if (column == 1) {
                editValue(Item(row), value, shift);
            }
        } else {
            int index = row - int(Item::Last);
            if (column == 1) {
                _userScale->editItem(index, value, shift);
            }
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Name, ///< Selects the name item.
        Mode, ///< Selects the mode item.
        Size, ///< Selects the size item.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for item.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Pointer to the item name; `nullptr` when no value is available.
     */
    const char *itemName(Item item) const {
        switch (item) {
        case Name:  return TXT_LIST_LABEL_NAME;
        case Mode:  return TXT_LIST_LABEL_MODE;
        case Size:  return TXT_LIST_LABEL_SIZE;
        case Last:  break;
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
            str(_userScale->name());
            break;
        case Mode:
            _userScale->printMode(str);
            break;
        case Size:
            _userScale->printSize(str);
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
        case Mode:
            _userScale->editMode(value, shift);
            break;
        case Size:
            _userScale->editSize(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Pointer to user scale; `nullptr` denotes that no object/resource is assigned.
     */
    UserScale *_userScale; ///< Pointer to user scale; `nullptr` denotes that no object/resource is assigned.
};
