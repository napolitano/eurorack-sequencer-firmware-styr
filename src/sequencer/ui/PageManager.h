/**
 * @file PageManager.h
 * @brief Declares the PageManager component used by the sequencer UI.
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

#include "Page.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

#include <array>
#include <functional>

struct Pages;

/**
 * @brief Owns the UI page stack and coordinates page transitions.
 */
class PageManager {
public:
    typedef std::function<void(Page *)> PageSwitchHandler;

    /**
     * @brief Constructs a PageManager instance.
     */
    PageManager() = default;

    /**
     * @brief Sets the pages.
     *
     * @param[in] pages Pages consumed by `setPages()`.
     */
    void setPages(Pages &pages) {
        _pages = &pages;
    }

    /**
     * @brief Returns the pages.
     *
     * @return Reference to the pages.
     */
    Pages &pages() {
        return *_pages;
    }

    /**
     * @brief Returns the top.
     *
     * @return Pointer to the top; `nullptr` when no value is available.
     */
    Page *top() const;
    /**
     * @brief Appends the supplied sample, item, or event.
     *
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    void push(Page *page);
    /**
     * @brief Removes and returns the next buffered item.
     */
    void pop();

    /**
     * @brief Returns the stack size.
     *
     * @return Number of pages currently present on the page stack.
     */
    int stackSize() const { return _pageStackPos + 1; }

    /**
     * @brief Resets the PageManager to its initial runtime state.
     *
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    void reset(Page *page);
    /**
     * @brief Replaces the currently stored value/object.
     *
     * @param[in] index Zero-based replace index.
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    void replace(int index, Page *page);

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);
    /**
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    void updateLeds(Leds &leds);

    /**
     * @brief Returns the fps.
     *
     * @return Requested UI refresh rate in frames per second.
     */
    int fps() const;

    /**
     * @brief Dispatches an event to the active UI/event handler.
     *
     * @param[in] event Event to process.
     */
    void dispatchEvent(Event &event);

    /**
     * @brief Sets the page switch handler.
     *
     * @param[in] pageSwitchHandler Callback invoked when the associated operation or event occurs.
     */
    void setPageSwitchHandler(PageSwitchHandler pageSwitchHandler) {
        _pageSwitchHandler = pageSwitchHandler;
    }

private:
    /**
     * @brief Notifies about page switch.
     *
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    void notifyPageSwitch(Page *page) {
        if (_pageSwitchHandler) {
            _pageSwitchHandler(page);
        }
    }

    /**
     * @brief Pointer to pages; `nullptr` denotes that no object/resource is assigned.
     */
    Pages *_pages = nullptr; ///< Pointer to pages; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Page stack size constant used by this component.
     */
    static const int PageStackSize = 8; ///< Maximum number of pages retained in the UI navigation stack.
    /**
     * @brief Pointer to page stack; `nullptr` denotes that no object/resource is assigned.
     */
    std::array<Page *, PageStackSize> _pageStack; ///< Pointer to page stack; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief UI value representing page stack pos.
     */
    int _pageStackPos = -1; ///< Index of the active page in the navigation stack; `-1` means the stack is empty.
    PageSwitchHandler _pageSwitchHandler; ///< Callback invoked for page switch activity.
};
