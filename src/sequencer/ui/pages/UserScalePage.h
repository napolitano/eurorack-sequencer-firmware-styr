/**
 * @file UserScalePage.h
 * @brief Declares the UserScalePage component used by the sequencer UI page.
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

#include "ListPage.h"

#include "ui/model/UserScaleListModel.h"

#include "model/UserScale.h"

class UserScalePage : public ListPage {
public:
    UserScalePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void setSelectedIndex(int index);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initUserScale();
    void copyUserScale();
    void pasteUserScale();
    void loadUserScale();
    void saveUserScale();

    void saveUserScaleToSlot(int slot);
    void loadUserScaleFromSlot(int slot);

    int _selectedIndex;
    UserScale *_userScale;
    UserScaleListModel _listModel;
};
