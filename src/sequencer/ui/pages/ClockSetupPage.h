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

/**
 * @brief Implements the clock setup page user-interface page.
 */
class ClockSetupPage : public ListPage {
public:
    /**
     * @brief Constructs a ClockSetupPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    ClockSetupPage(PageManager &manager, PageContext &context);

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;

private:
    ClockSetupListModel _listModel; ///< List model backing the list model controls shown on this page.
};
