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


class TextInputPage : public BasePage {
public:
    TextInputPage(PageManager &manager, PageContext &context);

    typedef std::function<void(bool, const char *)> ResultCallback;

    using BasePage::show;
    void show(const char *title, const char *text, size_t maxTextLength, ResultCallback callback);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void closeWithResult(bool result);

    void clear();
    void insert(char c);
    void backspace();
    void del();
    void moveLeft();
    void moveRight();

    const char *_title = nullptr;
    char _text[17];
    int _maxTextLength = 0;

    int _selectedIndex;
    int _cursorIndex;

    ResultCallback _callback;
};
