/**
 * @file MessageManager.cpp
 * @brief Implements the MessageManager component used by the sequencer UI.
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
#include "MessageManager.h"

#include "painters/WindowPainter.h"

#include "os/LockGuard.h"

#include "core/utils/StringUtils.h"


MessageManager::MessageManager() {
}

void MessageManager::showMessage(const char *text, uint32_t duration) {
    os::LockGuard lock(_mutex);

    StringUtils::copy(_text, text, sizeof(_text));
    _timeout = os::ticks() + os::time::ms(duration);
}

void MessageManager::update() {
    os::LockGuard lock(_mutex);

    if (os::ticks() > _timeout) {
        _timeout = 0;
    }
}

void MessageManager::draw(Canvas &canvas) {
    os::LockGuard lock(_mutex);

    if (!_timeout) {
        return;
    }

    WindowPainter::drawFrame(canvas, 16, 16, 224, 32);

    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);
    canvas.drawTextCentered(16, 16, 224, 32, _text);
}
