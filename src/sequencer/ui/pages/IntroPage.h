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

class IntroPage : public BasePage {
public:
    IntroPage(PageManager &manager, PageContext &context);

    virtual void draw(Canvas &canvas) override;

    virtual bool isModal() const override { return true; }
    // Close the modal intro page via user input (handled in IntroPage.cpp).
    virtual void keyPress(KeyPressEvent &event) override;

private:
    Intro _intro;

    uint32_t _lastTicks;
};
