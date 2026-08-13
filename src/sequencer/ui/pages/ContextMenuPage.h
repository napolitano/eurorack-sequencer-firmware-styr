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

/**
 * @brief Implements the context menu page user-interface page.
 */
class ContextMenuPage : public BasePage {
public:
    /**
     * @brief Constructs a ContextMenuPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    ContextMenuPage(PageManager &manager, PageContext &context);

    typedef std::function<void(int)> ResultCallback;

    using BasePage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] contextMenuModel Context-menu model rendered and controlled by this page.
     * @param[in] callback Callback invoked by the operation.
     */
    void show(ContextMenuModel &contextMenuModel, ResultCallback callback);

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;

    /**
     * @brief Reports whether modal.
     *
     * @return `true` if modal; otherwise `false`.
     */
    virtual bool isModal() const override { return true; }

    /**
     * @brief Reports whether a key is currently released.
     *
     * @param[in] event Event to process.
     */
    virtual void keyUp(KeyEvent &event) override;
    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    virtual void encoder(EncoderEvent &event) override;

protected:
    /**
     * @brief Closes and callback.
     *
     * @param[in] index Zero-based close and callback index.
     */
    void closeAndCallback(int index);

    /**
     * @brief Pointer to context menu model; `nullptr` denotes that no object/resource is assigned.
     */
    ContextMenuModel *_contextMenuModel; ///< Pointer to context menu model; `nullptr` denotes that no object/resource is assigned.
    ResultCallback _callback; ///< Callback that receives the operation result.
};
