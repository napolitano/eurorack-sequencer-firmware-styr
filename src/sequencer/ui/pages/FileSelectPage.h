/**
 * @file FileSelectPage.h
 * @brief Declares the FileSelectPage component used by the sequencer UI page.
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

#include "ui/model/FileSelectListModel.h"

#include "model/FileManager.h"

/**
 * @brief Implements the file select page user-interface page.
 */
class FileSelectPage : public ListPage {
public:
    /**
     * @brief Constructs a FileSelectPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    FileSelectPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, int)> ResultCallback;

    using ListPage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] title Title text displayed by the UI.
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] selectedSlot Storage slot selected when the page opens.
     * @param[in] allowEmpty Whether allow empty is enabled for this operation.
     * @param[in] callback Callback invoked by the operation.
     */
    void show(const char *title, FileType type, int selectedSlot, bool allowEmpty, ResultCallback callback);

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

    /**
     * @brief Pointer to title; `nullptr` denotes that no object/resource is assigned.
     */
    const char *_title; ///< Pointer to title; `nullptr` denotes that no object/resource is assigned.
    FileType _type; ///< File type associated with the storage operation.
    /**
     * @brief Whether allow empty is true in the current state.
     */
    bool _allowEmpty; ///< True when the file selector may accept an empty/no-file choice.
    ResultCallback _callback; ///< Callback that receives the operation result.

    FileSelectListModel _listModel; ///< List model backing the list model controls shown on this page.
};
