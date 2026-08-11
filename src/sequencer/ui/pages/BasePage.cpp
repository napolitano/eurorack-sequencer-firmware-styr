/**
 * @file BasePage.cpp
 * @brief Implements the BasePage component used by the sequencer UI page.
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
#include "BasePage.h"

#include "Pages.h"

#include "ui/model/ContextMenuModel.h"

BasePage::BasePage(PageManager &manager, PageContext &context) :
    Page(manager),
    _context(context),
    _model(context.model),
    _project(context.model.project()),
    _engine(context.engine)
{}

void BasePage::showMessage(const char *text, uint32_t duration) {
    _context.messageManager.showMessage(text, duration);
}

void BasePage::showContextMenu(const ContextMenu &contextMenu) {
    _context.contextMenu = contextMenu;
    _manager.pages().contextMenu.show(_context.contextMenu, _context.contextMenu.actionCallback());
}
