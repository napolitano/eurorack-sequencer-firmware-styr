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

class PageManager {
public:
    typedef std::function<void(Page *)> PageSwitchHandler;

    PageManager() = default;

    void setPages(Pages &pages) {
        _pages = &pages;
    }

    Pages &pages() {
        return *_pages;
    }

    Page *top() const;
    void push(Page *page);
    void pop();

    int stackSize() const { return _pageStackPos + 1; }

    void reset(Page *page);
    void replace(int index, Page *page);

    void draw(Canvas &canvas);
    void updateLeds(Leds &leds);

    int fps() const;

    void dispatchEvent(Event &event);

    void setPageSwitchHandler(PageSwitchHandler pageSwitchHandler) {
        _pageSwitchHandler = pageSwitchHandler;
    }

private:
    void notifyPageSwitch(Page *page) {
        if (_pageSwitchHandler) {
            _pageSwitchHandler(page);
        }
    }

    Pages *_pages = nullptr;
    static const int PageStackSize = 8;
    std::array<Page *, PageStackSize> _pageStack;
    int _pageStackPos = -1;
    PageSwitchHandler _pageSwitchHandler;
};