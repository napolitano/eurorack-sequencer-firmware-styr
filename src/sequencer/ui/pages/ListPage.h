/**
 * @file ListPage.h
 * @brief Declares the ListPage component used by the sequencer UI page.
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

#include "BasePage.h"

#include "ui/model/ListModel.h"

class ListPage : public BasePage {
public:
    ListPage(PageManager &manager, PageContext &context, ListModel &listModel);

    void setListModel(ListModel &listModel);

    virtual void show() override;

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

    int selectedRow() const { return _selectedRow; }
    void setSelectedRow(int selectedRow);

    bool edit() const { return _edit; }
    void setEdit(bool edit) { _edit = edit; }

    void setTopRow(int row);

protected:
    virtual void drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h);

private:
    void scrollTo(int row);

    static constexpr int LineHeight = 10;
    static constexpr int LineCount = 4;

    ListModel *_listModel;
    int _selectedRow = 0;
    int _displayRow = 0;
    bool _edit = false;
};
