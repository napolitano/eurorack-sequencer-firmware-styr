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

/**
 * @brief Implements the user scale page user-interface page.
 */
class UserScalePage : public ListPage {
public:
    /**
     * @brief Constructs a UserScalePage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    UserScalePage(PageManager &manager, PageContext &context);

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
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;

private:
    /**
     * @brief Sets the selected index.
     *
     * @param[in] index Zero-based selected index index.
     */
    void setSelectedIndex(int index);

    /**
     * @brief Opens the context menu for the current selection.
     */
    void contextShow();
    /**
     * @brief Executes the selected context-menu action.
     *
     * @param[in] index Zero-based context action index.
     */
    void contextAction(int index);
    /**
     * @brief Returns context action enabled.
     *
     * @param[in] index Zero-based context action enabled index.
     *
     * @return `true` if context action enabled; otherwise `false`.
     */
    bool contextActionEnabled(int index) const;

    /**
     * @brief Initializes user scale.
     */
    void initUserScale();
    /**
     * @brief Copies user scale.
     */
    void copyUserScale();
    /**
     * @brief Pastes user scale.
     */
    void pasteUserScale();
    /**
     * @brief Loads user scale.
     */
    void loadUserScale();
    /**
     * @brief Saves user scale.
     */
    void saveUserScale();

    /**
     * @brief Saves user scale to slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     */
    void saveUserScaleToSlot(int slot);
    /**
     * @brief Loads user scale from slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     */
    void loadUserScaleFromSlot(int slot);

    /**
     * @brief Zero-based selected index; a negative/sentinel value represents no selection where applicable.
     */
    int _selectedIndex; ///< Zero-based selected index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Pointer to user scale; `nullptr` denotes that no object/resource is assigned.
     */
    UserScale *_userScale; ///< Pointer to user scale; `nullptr` denotes that no object/resource is assigned.
    UserScaleListModel _listModel; ///< List model backing the list model controls shown on this page.
};
