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

/**
 * @brief Implements the track page user-interface page.
 */
class TrackPage : public ListPage {
public:
    /**
     * @brief Constructs a TrackPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    TrackPage(PageManager &manager, PageContext &context);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() override;
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    virtual void exit() override;

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
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;

private:
    /**
     * @brief Sets the track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void setTrack(Track &track);

    /**
     * @brief Opens the context menu for the current selection.
     */
    void contextShow();
    /**
     * @brief Executes the selected context-menu action.
     *
     * @param[in] index Zero-based context action index.
     */
    void contextAction(int index);
    /**
     * @brief Returns context action enabled.
     *
     * @param[in] index Zero-based context action enabled index.
     *
     * @return `true` if context action enabled; otherwise `false`.
     */
    bool contextActionEnabled(int index) const;

    /**
     * @brief Initializes track setup.
     */
    void initTrackSetup();
    /**
     * @brief Copies track setup.
     */
    void copyTrackSetup();
    /**
     * @brief Pastes track setup.
     */
    void pasteTrackSetup();
    /**
     * @brief Initializes a routing entry with default values.
     */
    void initRoute();

    /**
     * @brief Pointer to list model; `nullptr` denotes that no object/resource is assigned.
     */
    RoutableListModel *_listModel; ///< Pointer to list model; `nullptr` denotes that no object/resource is assigned.
    NoteTrackListModel _noteTrackListModel; ///< List model backing the note track controls shown on this page.
    CurveTrackListModel _curveTrackListModel; ///< List model backing the curve track controls shown on this page.
    MidiCvTrackListModel _midiCvTrackListModel; ///< List model backing the midi cv track controls shown on this page.
    LfoTrackListModel _lfoTrackListModel; ///< List model backing the lfo track controls shown on this page.
};
