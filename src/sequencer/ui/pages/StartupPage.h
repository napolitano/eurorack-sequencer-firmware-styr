/**
 * @file StartupPage.h
 * @brief Declares the StartupPage component used by the sequencer UI page.
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

class StartupPage : public BasePage {
public:
    enum class IndicatorDirection {
        LeftToRight,
        RightToLeft,
        CenterOut
    };

    StartupPage(PageManager &manager, PageContext &context);

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }
    virtual void drawActivityIndicator(Canvas &canvas, int yStart, int minWidth, int maxWidth, int distance, int indicatorCount, int throttle, bool filled, bool modulateColor, IndicatorDirection direction);

private:
    enum class State {
        Initial,
        Loading,
        Ready,
    };

    float time() const;
    float relTime() const { return time() / LoadTime; }

    static constexpr int LoadTime = 2;

    uint32_t _startTicks;
    State _state = State::Initial;
};
