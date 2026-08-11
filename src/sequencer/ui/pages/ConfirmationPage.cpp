/**
 * @file ConfirmationPage.cpp
 * @brief Implements the ConfirmationPage component used by the sequencer UI page.
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
#include "Config.h"
#include "ConfirmationPage.h"

#include "ui/painters/WindowPainter.h"

enum class Function {
    No  = 3,
    Yes = 4,
};

static const char *functionNames[] = {
	nullptr,
	nullptr,
	nullptr,
	TXT_MENU_NO,
	TXT_MENU_YES
};


ConfirmationPage::ConfirmationPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void ConfirmationPage::show(const char *text, ResultCallback callback) {
    _text = text;
    _callback = callback;
    BasePage::show();
}

void ConfirmationPage::enter() {
}

void ConfirmationPage::exit() {
}

void ConfirmationPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(UI_COLOR_ACTIVE);

    canvas.drawTextCentered(0, 32 - 4, Width, 8, _text);
}

void ConfirmationPage::updateLeds(Leds &leds) {
}

void ConfirmationPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::No:
            closeWithResult(false);
            break;
        case Function::Yes:
            closeWithResult(true);
            break;
        }
    }
}

void ConfirmationPage::closeWithResult(bool result) {
    BasePage::close();
    if (_callback) {
        _callback(result);
    }
}
