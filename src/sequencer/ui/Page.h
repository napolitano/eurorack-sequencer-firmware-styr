/**
 * @file Page.h
 * @brief Declares the Page component used by the sequencer UI.
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

#include "Config.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

class PageManager;

class Page {
public:
    Page(PageManager &manager);

    virtual void enter() {}
    virtual void exit() {}

    virtual void draw(Canvas &canvas) {}
    virtual void updateLeds(Leds &leds) {}

    virtual int fps() const { return CONFIG_DEFAULT_UI_FPS; }

    virtual bool isModal() const { return false; }

    // Event handlers
    virtual void keyDown(KeyEvent &event) {}
    virtual void keyUp(KeyEvent &event) {}
    virtual void keyPress(KeyPressEvent &event) {}
    virtual void encoder(EncoderEvent &event) {}
    virtual void midi(MidiEvent &event) {}

    virtual void show();
    virtual void close();
    virtual bool isTop();

    virtual void dispatchEvent(Event &event);

protected:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    PageManager &_manager;
};
