/**
 * @file BusyPage.h
 * @brief Declares the BusyPage component used by the sequencer UI page.
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

/**
 * @brief Implements the busy page user-interface page.
 */
class BusyPage : public BasePage {
public:
    /**
     * @brief Constructs a BusyPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    BusyPage(PageManager &manager, PageContext &context);

    using BasePage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     */
    void show(const char *text);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() override;
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    virtual void exit() override;

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;
    /**
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    virtual void updateLeds(Leds &leds) override;

    /**
     * @brief Reports whether modal.
     *
     * @return `true` if modal; otherwise `false`.
     */
    virtual bool isModal() const override { return true; }

private:
    /**
     * @brief Pointer to text; `nullptr` denotes that no object/resource is assigned.
     */
    const char *_text; ///< Pointer to text; `nullptr` denotes that no object/resource is assigned.
};
