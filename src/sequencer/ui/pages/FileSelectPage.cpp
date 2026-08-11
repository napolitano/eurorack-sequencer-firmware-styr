/**
 * @file FileSelectPage.cpp
 * @brief Implements the FileSelectPage component used by the sequencer UI page.
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
#include "FileSelectPage.h"

#include "ui/painters/WindowPainter.h"

enum class Function {
    Cancel  = 3,
    OK      = 4,
};

static const char *functionNames[] = {
	nullptr,
	nullptr,
	nullptr,
	TXT_MENU_CANCEL,
	TXT_MENU_OK
};


FileSelectPage::FileSelectPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void FileSelectPage::show(const char *title, FileType type, int selectedSlot, bool allowEmpty, ResultCallback callback) {
    _title = title;
    _type = type;
    _allowEmpty = allowEmpty;
    _callback = callback;
    _listModel.setType(type);
    setSelectedRow(clamp(selectedSlot, 0, _listModel.rows() - 1));
    ListPage::show();
}

void FileSelectPage::enter() {
}

void FileSelectPage::exit() {
}

void FileSelectPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, _title);
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    ListPage::draw(canvas);
}

void FileSelectPage::updateLeds(Leds &leds) {
}

void FileSelectPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Cancel:
            closeWithResult(false);
            break;
        case Function::OK:
            closeWithResult(true);
            break;
        }
    }

    if (key.is(Key::Encoder)) {
        closeWithResult(true);
        return;
    }

    ListPage::keyPress(event);
}

void FileSelectPage::closeWithResult(bool result) {
    // cancel if empty slot is selected but not allowed to be
    if (result && !_allowEmpty) {
        FileManager::SlotInfo info;
        FileManager::slotInfo(_type, selectedRow(), info);
        if (!info.used) {
            return;
        }
    }

    Page::close();
    if (_callback) {
        _callback(result, selectedRow());
    }
}
