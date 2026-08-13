/**
 * @file GeneratorSelectPage.h
 * @brief Declares the GeneratorSelectPage component used by the sequencer UI page.
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

#include "ui/model/GeneratorSelectListModel.h"

/**
 * @brief Implements the generator select page user-interface page.
 */
class GeneratorSelectPage : public ListPage {
public:
    /**
     * @brief Constructs a GeneratorSelectPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    GeneratorSelectPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, Generator::Mode)> ResultCallback;

    using ListPage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] callback Callback invoked by the operation.
     */
    void show(ResultCallback callback);

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

    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;

private:
    /**
     * @brief Closes the active page/dialog and commits its result.
     *
     * @param[in] result Whether result is enabled for this operation.
     */
    void closeWithResult(bool result);

    ResultCallback _callback; ///< Callback that receives the operation result.
    GeneratorSelectListModel _listModel; ///< List model backing the list model controls shown on this page.
};
