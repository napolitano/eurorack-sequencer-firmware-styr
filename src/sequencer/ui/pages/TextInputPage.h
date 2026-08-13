/**
 * @file TextInputPage.h
 * @brief Declares the TextInputPage component used by the sequencer UI page.
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
 * @brief Implements the text input page user-interface page.
 */
class TextInputPage : public BasePage {
public:
    /**
     * @brief Constructs a TextInputPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    TextInputPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, const char *)> ResultCallback;

    using BasePage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] title Title text displayed by the UI.
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     * @param[in] maxTextLength Maximum number of user-editable characters accepted by the page.
     * @param[in] callback Callback invoked by the operation.
     */
    void show(const char *title, const char *text, size_t maxTextLength, ResultCallback callback);

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
     * @brief Reports whether modal.
     *
     * @return `true` if modal; otherwise `false`.
     */
    virtual bool isModal() const override { return true; }

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
     * @brief Closes the active page/dialog and commits its result.
     *
     * @param[in] result Whether result is enabled for this operation.
     */
    void closeWithResult(bool result);

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();
    /**
     * @brief Inserts the supplied character/item at the current position.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    void insert(char c);
    /**
     * @brief Deletes the item/character before the current cursor.
     */
    void backspace();
    /**
     * @brief Deletes the item/character at the current cursor.
     */
    void del();
    /**
     * @brief Moves the cursor/selection one position left.
     */
    void moveLeft();
    /**
     * @brief Moves the cursor/selection one position right.
     */
    void moveRight();

    /**
     * @brief Pointer to title; `nullptr` denotes that no object/resource is assigned.
     */
    const char *_title = nullptr; ///< Pointer to title; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief UI value representing text.
     */
    char _text[17]; ///< Null-terminated text buffer owned by the UI component.
    /**
     * @brief UI value representing max text length.
     */
    int _maxTextLength = 0; ///< Maximum number of editable characters accepted by the text-input page.

    /**
     * @brief Zero-based selected index; a negative/sentinel value represents no selection where applicable.
     */
    int _selectedIndex; ///< Zero-based selected index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Zero-based cursor index; a negative/sentinel value represents no selection where applicable.
     */
    int _cursorIndex; ///< Zero-based cursor index; a negative/sentinel value represents no selection where applicable.

    ResultCallback _callback; ///< Callback that receives the operation result.
};
