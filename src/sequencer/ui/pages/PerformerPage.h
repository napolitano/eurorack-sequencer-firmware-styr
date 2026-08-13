/**
 * @file PerformerPage.h
 * @brief Declares the PerformerPage component used by the sequencer UI page.
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
 * @brief Implements the performer page user-interface page.
 */
class PerformerPage : public BasePage {
public:
    /**
     * @brief Constructs a PerformerPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    PerformerPage(PageManager &manager, PageContext &context);

    /**
     * @brief Sets the modal.
     *
     * @param[in] modal `true` to enable/set modal; `false` to disable/clear it.
     */
    void setModal(bool modal) { _modal = modal; }

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
    virtual bool isModal() const override { return _modal; }

    /**
     * @brief Reports whether a key is currently held.
     *
     * @param[in] event Event to process.
     */
    virtual void keyDown(KeyEvent &event) override;
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

private:
    /**
     * @brief Refreshes fill state for the current project/track context.
     */
    void updateFills();

    /**
     * @brief Whether modal is true in the current state.
     */
    bool _modal = false; ///< True while the page owns modal input and blocks normal page navigation.
    /**
     * @brief Whether latching is true in the current state.
     */
    bool _latching = false; ///< True while the page is collecting a latched pattern/performance request.
    /**
     * @brief Whether syncing is true in the current state.
     */
    bool _syncing = false; ///< True while the page is collecting a request that will commit at a synchronization boundary.
};
