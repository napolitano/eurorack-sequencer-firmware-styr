/**
 * @file MessageManager.h
 * @brief Declares the MessageManager component used by the sequencer UI.
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

#include "os/os.h"

#include "core/gfx/Canvas.h"

#include <cstdint>

class MessageManager {
public:
    MessageManager();

    void showMessage(const char *text, uint32_t duration = 1000);

    void update();

    void draw(Canvas &canvas);

private:
    char _text[64];
    uint32_t _timeout = 0;

    os::Mutex _mutex;
};
