/**
 * @file IntroPage.h
 * @brief Declares the IntroPage component used by the sequencer UI page.
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

#include "intro/Intro.h"

/**
 * @brief Implements the intro page user-interface page.
 */
class IntroPage : public BasePage {
public:
    /**
     * @brief Constructs a IntroPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    IntroPage(PageManager &manager, PageContext &context);

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
    // Close the modal intro page via user input (handled in IntroPage.cpp).
    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;

private:
    Intro _intro; ///< Intro-screen renderer/state used by the intro page.

    /**
     * @brief Most recently observed ticks.
     */
    uint32_t _lastTicks; ///< System tick of the previous intro-animation update.
};
