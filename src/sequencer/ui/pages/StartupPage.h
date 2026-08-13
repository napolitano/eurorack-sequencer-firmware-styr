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

/**
 * @brief Implements the startup page user-interface page.
 */
class StartupPage : public BasePage {
public:
    /**
     * @brief Enumerates the supported indicator direction values.
     */
    enum class IndicatorDirection {
        LeftToRight, ///< Selects the left to right indicator direction.
        RightToLeft, ///< Selects the right to left indicator direction.
        CenterOut ///< Selects the center out indicator direction.
    };

    /**
     * @brief Constructs a StartupPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    StartupPage(PageManager &manager, PageContext &context);

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
     * @brief Draws activity indicator.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] yStart Vertical pixel coordinate at which the startup indicator row begins.
     * @param[in] minWidth Minimum indicator width in pixels.
     * @param[in] maxWidth Maximum indicator width in pixels.
     * @param[in] distance Horizontal spacing between startup indicators, in pixels.
     * @param[in] indicatorCount Number of startup indicators to render.
     * @param[in] throttle Frame/update throttling factor used by the startup animation.
     * @param[in] filled Whether filled is enabled for this operation.
     * @param[in] modulateColor Whether modulate color is enabled for this operation.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     */
    virtual void drawActivityIndicator(Canvas &canvas, int yStart, int minWidth, int maxWidth, int distance, int indicatorCount, int throttle, bool filled, bool modulateColor, IndicatorDirection direction);

private:
    /**
     * @brief Enumerates the supported state values.
     */
    enum class State {
        Initial, ///< Selects the initial state.
        Loading, ///< Selects the loading state.
        Ready, ///< Selects the ready state.
    };

    /**
     * @brief Returns the time.
     *
     * @return Current startup animation time.
     */
    float time() const;
    /**
     * @brief Returns the rel time.
     *
     * @return Normalized/relative startup animation time.
     */
    float relTime() const { return time() / LoadTime; }

    /**
     * @brief Load time constant used by this component.
     */
    static constexpr int LoadTime = 2; ///< Startup-page minimum display time, in seconds.

    /**
     * @brief UI value representing start ticks.
     */
    uint32_t _startTicks; ///< Scheduler tick captured when the startup page became active.
    State _state = State::Initial; ///< Current runtime state.
};
