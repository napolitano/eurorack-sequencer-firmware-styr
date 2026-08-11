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

class UserScaleListModel : public ListModel {
public:
    UserScaleListModel()
    {}

    void setUserScale(UserScale &userScale) {
        _userScale = &userScale;
    }

    virtual int rows() const override {
        return Item::Last + _userScale->size();
    }

    virtual int columns() const override {
        return 2;
    }

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
    enum Item {
        Name,
        Mode,
        Size,
        Last
    };

    const char *itemName(Item item) const {
        switch (item) {
        case Name:  return TXT_LIST_LABEL_NAME;
        case Mode:  return TXT_LIST_LABEL_MODE;
        case Size:  return TXT_LIST_LABEL_SIZE;
        case Last:  break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

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

    UserScale *_userScale;
};
