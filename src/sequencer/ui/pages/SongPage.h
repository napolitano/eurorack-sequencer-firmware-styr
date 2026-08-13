/**
 * @file SongPage.h
 * @brief Declares the SongPage component used by the sequencer UI page.
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

/**
 * @brief Implements the song page user-interface page.
 */
class SongPage : public BasePage {
public:
    /**
     * @brief Constructs a SongPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    SongPage(PageManager &manager, PageContext &context);

    /**
     * @brief Resets the SongPage to its initial runtime state.
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
     * @brief Number of slot entries supported by the implementation.
     */
    static const int SlotCount = CONFIG_SONG_SLOT_COUNT; ///< Number of slot entries supported by the implementation.
    /**
     * @brief Number of row entries supported by the implementation.
     */
    static const int RowCount = 4; ///< Number of row entries supported by the implementation.

    /**
     * @brief Sets the selected slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     */
    void setSelectedSlot(int slot);
    /**
     * @brief Moves/reorders the selected storage slot.
     *
     * @param[in] offset Offset applied in the domain defined by the operation.
     * @param[in] moveSlot Whether move slot is enabled for this operation.
     */
    void moveSelectedSlot(int offset, bool moveSlot);
    /**
     * @brief Scrolls the view so the requested item is visible.
     *
     * @param[in] row Zero-based row index.
     */
    void scrollTo(int row);

    /**
     * @brief Returns the pressed track keys.
     *
     * @return Bit mask of currently pressed track-selection keys.
     */
    uint8_t pressedTrackKeys() const;

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
     * @brief Initializes song.
     */
    void initSong();

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Idle, ///< Selects the idle mode.
        Chain ///< Selects the chain mode.
    };

    Mode _mode; ///< Active mode controlling the behavior of `SongPage`.
    /**
     * @brief UI value representing selected slot.
     */
    int _selectedSlot = -1; ///< Selected song slot index; `-1` means no slot is selected.
    /**
     * @brief UI value representing display row.
     */
    int _displayRow = 0; ///< Zero-based first row currently visible in the scrolling list.
};
