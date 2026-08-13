/**
 * @file ListPage.h
 * @brief Declares the ListPage component used by the sequencer UI page.
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

#include "ui/model/ListModel.h"

/**
 * @brief Implements the list page user-interface page.
 */
class ListPage : public BasePage {
public:
    /**
     * @brief Constructs a ListPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     * @param[in] listModel List model supplying rows/items to the UI.
     */
    ListPage(PageManager &manager, PageContext &context, ListModel &listModel);

    /**
     * @brief Sets the list model.
     *
     * @param[in] listModel List model supplying rows/items to the UI.
     */
    void setListModel(ListModel &listModel);

    /**
     * @brief Makes the page, dialog, or message visible.
     */
    virtual void show() override;

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
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    virtual void encoder(EncoderEvent &event) override;

    /**
     * @brief Selects ed row.
     *
     * @return Currently selected list row.
     */
    int selectedRow() const { return _selectedRow; }
    /**
     * @brief Sets the selected row.
     *
     * @param[in] selectedRow Selected row consumed by `setSelectedRow()`.
     */
    void setSelectedRow(int selectedRow);

    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @return `true` if edit; otherwise `false`.
     */
    bool edit() const { return _edit; }
    /**
     * @brief Sets the edit.
     *
     * @param[in] edit `true` to enable/set edit; `false` to disable/clear it.
     */
    void setEdit(bool edit) { _edit = edit; }

    /**
     * @brief Sets the top row.
     *
     * @param[in] row Zero-based row index.
     */
    void setTopRow(int row);

protected:
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
    virtual void drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h);

private:
    /**
     * @brief Scrolls the view so the requested item is visible.
     *
     * @param[in] row Zero-based row index.
     */
    void scrollTo(int row);

    /**
     * @brief Line height constant used by this component.
     */
    static constexpr int LineHeight = 10; ///< Vertical spacing between list rows, in pixels.
    /**
     * @brief Number of line entries supported by the implementation.
     */
    static constexpr int LineCount = 4; ///< Number of line entries supported by the implementation.

    /**
     * @brief Pointer to list model; `nullptr` denotes that no object/resource is assigned.
     */
    ListModel *_listModel; ///< Pointer to list model; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief UI value representing selected row.
     */
    int _selectedRow = 0; ///< Zero-based row currently selected by the user.
    /**
     * @brief UI value representing display row.
     */
    int _displayRow = 0; ///< Zero-based first row currently visible in the scrolling list.
    /**
     * @brief Whether edit is true in the current state.
     */
    bool _edit = false; ///< True while the list page is editing the selected item rather than navigating rows.
};
