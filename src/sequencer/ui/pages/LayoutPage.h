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

/**
 * @brief Implements the layout page user-interface page.
 */
class LayoutPage : public ListPage {
public:
    /**
     * @brief Constructs a LayoutPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    LayoutPage(PageManager &manager, PageContext &context);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() override;

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;

    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;

private:
    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        TrackMode, ///< Selects the track mode mode.
        LinkTrack, ///< Selects the link track mode.
        GateOutput, ///< Selects the gate output mode.
        CvOutput, ///< Selects the cv output mode.
    };

    /**
     * @brief Returns the display name for mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the mode name; `nullptr` when no value is available.
     */
    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::TrackMode:   return "TRACK MODE"; break;
        case Mode::LinkTrack:   return "LINK TRACK"; break;
        case Mode::GateOutput:  return "GATE OUTPUT"; break;
        case Mode::CvOutput:    return "CV OUTPUT"; break;
        }
        return nullptr;
    }

    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode);

    Mode _mode = Mode::TrackMode; ///< Active mode controlling the behavior of `LayoutPage`.
    TrackModeListModel _trackModeListModel; ///< List model backing the track mode controls shown on this page.
    LinkTrackListModel _linkTrackListModel; ///< List model backing the link track controls shown on this page.
    GateOutputListModel _gateOutputListModel; ///< List model backing the gate output controls shown on this page.
    CvOutputListModel _cvOutputListModel; ///< List model backing the cv output controls shown on this page.
};
