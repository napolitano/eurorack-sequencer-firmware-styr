/**
 * @file ClockSetupPage.h
 * @brief Declares the ClockSetupPage component used by the sequencer UI page.
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

#include "ui/model/ClockSetupListModel.h"

class ClockSetupPage : public ListPage {
public:
    ClockSetupPage(PageManager &manager, PageContext &context);

    virtual void draw(Canvas &canvas) override;

private:
    ClockSetupListModel _listModel;
};
