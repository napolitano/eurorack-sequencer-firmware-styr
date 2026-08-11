/**
 * @file LayoutPage.h
 * @brief Declares the LayoutPage component used by the sequencer UI page.
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

#include "ui/model/TrackModeListModel.h"
#include "ui/model/LinkTrackListModel.h"
#include "ui/model/GateOutputListModel.h"
#include "ui/model/CvOutputListModel.h"

class LayoutPage : public ListPage {
public:
    LayoutPage(PageManager &manager, PageContext &context);

    virtual void enter() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    enum class Mode : uint8_t {
        TrackMode,
        LinkTrack,
        GateOutput,
        CvOutput,
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::TrackMode:   return "TRACK MODE"; break;
        case Mode::LinkTrack:   return "LINK TRACK"; break;
        case Mode::GateOutput:  return "GATE OUTPUT"; break;
        case Mode::CvOutput:    return "CV OUTPUT"; break;
        }
        return nullptr;
    }

    void setMode(Mode mode);

    Mode _mode = Mode::TrackMode;
    TrackModeListModel _trackModeListModel;
    LinkTrackListModel _linkTrackListModel;
    GateOutputListModel _gateOutputListModel;
    CvOutputListModel _cvOutputListModel;
};
