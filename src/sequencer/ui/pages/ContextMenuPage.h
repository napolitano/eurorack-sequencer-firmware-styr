/**
 * @file ContextMenuPage.h
 * @brief Declares the ContextMenuPage component used by the sequencer UI page.
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

#include "ui/model/ContextMenuModel.h"

class ContextMenuPage : public BasePage {
public:
    ContextMenuPage(PageManager &manager, PageContext &context);

    typedef std::function<void(int)> ResultCallback;

    using BasePage::show;
    void show(ContextMenuModel &contextMenuModel, ResultCallback callback);

    virtual void draw(Canvas &canvas) override;

    virtual bool isModal() const override { return true; }

    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

protected:
    void closeAndCallback(int index);

    ContextMenuModel *_contextMenuModel;
    ResultCallback _callback;
};
