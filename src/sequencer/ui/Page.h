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

/**
 * @brief Implements the page user-interface page.
 */
class Page {
public:
    /**
     * @brief Constructs a Page instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     */
    Page(PageManager &manager);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() {}
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    virtual void exit() {}

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) {}
    /**
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    virtual void updateLeds(Leds &leds) {}

    /**
     * @brief Returns the fps.
     *
     * @return Requested UI refresh rate in frames per second.
     */
    virtual int fps() const { return CONFIG_DEFAULT_UI_FPS; }

    /**
     * @brief Reports whether modal.
     *
     * @return `true` if modal; otherwise `false`.
     */
    virtual bool isModal() const { return false; }

    // Event handlers
    /**
     * @brief Reports whether a key is currently held.
     *
     * @param[in] event Event to process.
     */
    virtual void keyDown(KeyEvent &event) {}
    /**
     * @brief Reports whether a key is currently released.
     *
     * @param[in] event Event to process.
     */
    virtual void keyUp(KeyEvent &event) {}
    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) {}
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    virtual void encoder(EncoderEvent &event) {}
    /**
     * @brief Returns or processes the MIDI interface associated with this component.
     *
     * @param[in] event Event to process.
     */
    virtual void midi(MidiEvent &event) {}

    /**
     * @brief Makes the page, dialog, or message visible.
     */
    virtual void show();
    /**
     * @brief Closes the active page, file, dialog, or resource.
     */
    virtual void close();
    /**
     * @brief Reports whether top.
     *
     * @return `true` if top; otherwise `false`.
     */
    virtual bool isTop();

    /**
     * @brief Dispatches an event to the active UI/event handler.
     *
     * @param[in] event Event to process.
     */
    virtual void dispatchEvent(Event &event);

protected:
    /**
     * @brief Width constant used by this component.
     */
    static constexpr int Width = CONFIG_LCD_WIDTH; ///< Display/framebuffer width in pixels.
    /**
     * @brief Height constant used by this component.
     */
    static constexpr int Height = CONFIG_LCD_HEIGHT; ///< Display/framebuffer height in pixels.

    /**
     * @brief Reference to manager owned by another component.
     */
    PageManager &_manager; ///< Reference to manager owned by another component.
};
