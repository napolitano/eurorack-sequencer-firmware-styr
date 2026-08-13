/**
 * @file ProjectPage.h
 * @brief Declares the ProjectPage component used by the sequencer UI page.
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

#include "ui/model/ProjectListModel.h"

/**
 * @brief Implements the project page user-interface page.
 */
class ProjectPage : public ListPage {
public:
    /**
     * @brief Constructs a ProjectPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    ProjectPage(PageManager &manager, PageContext &context);

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

private:
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
     * @brief Initializes project.
     */
    void initProject();
    /**
     * @brief Loads project.
     */
    void loadProject();
    /**
     * @brief Saves project.
     */
    void saveProject();
    /**
     * @brief Saves as project.
     */
    void saveAsProject();
    /**
     * @brief Initializes a routing entry with default values.
     */
    void initRoute();

    /**
     * @brief Saves project to slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     */
    void saveProjectToSlot(int slot);
    /**
     * @brief Loads project from slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     */
    void loadProjectFromSlot(int slot);

    ProjectListModel _listModel; ///< List model backing the list model controls shown on this page.
};
