/**
 * @file TopPage.h
 * @brief Declares the TopPage component used by the sequencer UI page.
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

#include "ui/PageKeyMap.h"

#include "model/Routing.h"

/**
 * @brief Implements the top page user-interface page.
 */
class TopPage : public BasePage {
public:
    /**
     * @brief Constructs a TopPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    TopPage(PageManager &manager, PageContext &context);

    /**
     * @brief Initializes the TopPage and its runtime resources.
     */
    void init();

    /**
     * @brief Adjusts the route from a UI edit delta.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] trackIndex Zero-based track index.
     */
    void editRoute(Routing::Target target, int trackIndex);

    /**
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    virtual void updateLeds(Leds &leds) override;

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
     * @brief Enumerates the supported mode values.
     */
    enum Mode : uint8_t {
        // main modes
        Project         = PageKeyMap::Project, ///< Selects the project mode.
        Layout          = PageKeyMap::Layout, ///< Selects the layout mode.
        Track           = PageKeyMap::Track, ///< Selects the track mode.
        Sequence        = PageKeyMap::Sequence, ///< Selects the sequence mode.
        SequenceEdit    = PageKeyMap::SequenceEdit, ///< Selects the sequence edit mode.
        Song            = PageKeyMap::Song, ///< Selects the song mode.
        Routing         = PageKeyMap::Routing, ///< Selects the routing mode.
        MidiOutput      = PageKeyMap::MidiOutput, ///< Selects the midi output mode.
        Pattern         = PageKeyMap::Pattern, ///< Selects the pattern mode.
        Performer       = PageKeyMap::Performer, ///< Selects the performer mode.
        Overview        = PageKeyMap::Overview, ///< Selects the overview mode.
        Clock           = PageKeyMap::Clock, ///< Selects the clock mode.

        // aux modes
        UserScale       = PageKeyMap::UserScale, ///< Selects the user scale mode.
        Monitor         = PageKeyMap::Monitor, ///< Selects the monitor mode.
        System          = PageKeyMap::System, ///< Selects the system mode.

        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode);
    /**
     * @brief Sets the main page.
     *
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    void setMainPage(Page &page);

    /**
     * @brief Sets the sequence page.
     */
    void setSequencePage();
    /**
     * @brief Sets the sequence edit page.
     */
    void setSequenceEditPage();

    Mode _mode; ///< Active mode controlling the behavior of `TopPage`.
    /**
     * @brief Most recently observed mode.
     */
    Mode _lastMode; ///< Previously rendered top-page mode, used to detect mode changes that require a full refresh.
};
