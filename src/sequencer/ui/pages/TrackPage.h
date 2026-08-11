/**
 * @file TrackPage.h
 * @brief Declares the TrackPage component used by the sequencer UI page.
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

#include "ListPage.h"

#include "ui/model/NoteTrackListModel.h"
#include "ui/model/CurveTrackListModel.h"
#include "ui/model/MidiCvTrackListModel.h"
#include "ui/model/LfoTrackListModel.h"

class TrackPage : public ListPage {
public:
    TrackPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void setTrack(Track &track);

    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initTrackSetup();
    void copyTrackSetup();
    void pasteTrackSetup();
    void initRoute();

    RoutableListModel *_listModel;
    NoteTrackListModel _noteTrackListModel;
    CurveTrackListModel _curveTrackListModel;
    MidiCvTrackListModel _midiCvTrackListModel;
    LfoTrackListModel _lfoTrackListModel;
};
