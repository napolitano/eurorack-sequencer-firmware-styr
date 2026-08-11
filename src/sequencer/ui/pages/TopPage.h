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

class TopPage : public BasePage {
public:
    TopPage(PageManager &manager, PageContext &context);

    void init();

    void editRoute(Routing::Target target, int trackIndex);

    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum Mode : uint8_t {
        // main modes
        Project         = PageKeyMap::Project,
        Layout          = PageKeyMap::Layout,
        Track           = PageKeyMap::Track,
        Sequence        = PageKeyMap::Sequence,
        SequenceEdit    = PageKeyMap::SequenceEdit,
        Song            = PageKeyMap::Song,
        Routing         = PageKeyMap::Routing,
        MidiOutput      = PageKeyMap::MidiOutput,
        Pattern         = PageKeyMap::Pattern,
        Performer       = PageKeyMap::Performer,
        Overview        = PageKeyMap::Overview,
        Clock           = PageKeyMap::Clock,

        // aux modes
        UserScale       = PageKeyMap::UserScale,
        Monitor         = PageKeyMap::Monitor,
        System          = PageKeyMap::System,

        Last,
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
