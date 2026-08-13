/**
 * @file RoutingPage.h
 * @brief Declares the RoutingPage component used by the sequencer UI page.
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

#include "ui/model/RouteListModel.h"

#include "model/Routing.h"

#include "engine/MidiLearn.h"

/**
 * @brief Implements the routing page user-interface page.
 */
class RoutingPage : public ListPage {
public:
    /**
     * @brief Constructs a RoutingPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    RoutingPage(PageManager &manager, PageContext &context);

    /**
     * @brief Resets the RoutingPage to its initial runtime state.
     */
    void reset();

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

    /**
     * @brief Shows route.
     *
     * @param[in] routeIndex Zero-based route index.
     * @param[in] initialValue Initial value shown when the editor/page opens.
     */
    void showRoute(int routeIndex, const Routing::Route *initialValue = nullptr);

private:
    /**
     * @brief Draws cell.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    virtual void drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h) override;

    /**
     * @brief Selects route.
     *
     * @param[in] routeIndex Zero-based route index.
     */
    void selectRoute(int routeIndex);
    /**
     * @brief Assigns the most recently learned MIDI control to the selected target.
     *
     * @param[in] result Result object/value populated by the operation.
     */
    void assignMidiLearn(const MidiLearn::Result &result);

    RouteListModel _routeListModel; ///< List model backing the route controls shown on this page.
    /**
     * @brief Pointer to route; `nullptr` denotes that no object/resource is assigned.
     */
    Routing::Route *_route; ///< Pointer to route; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Zero-based route index; a negative/sentinel value represents no selection where applicable.
     */
    uint8_t _routeIndex; ///< Zero-based route index; a negative/sentinel value represents no selection where applicable.
    Routing::Route _editRoute; ///< Working copy of the route being edited before it is committed to the project.
};
