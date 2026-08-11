/**
 * @file SongPainter.h
 * @brief Declares the SongPainter component used by the sequencer UI painter.
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

#include "core/gfx/Canvas.h"

class SongPainter {
public:
    static void drawArrowDown(Canvas &canvas, int x, int y, int w);
    static void drawArrowUp(Canvas &canvas, int x, int y, int w);
    static void drawArrowRight(Canvas &canvas, int x, int y, int w, int h);
    static void drawProgress(Canvas &canvas, int x, int y, int w, int h, float progress);
};
