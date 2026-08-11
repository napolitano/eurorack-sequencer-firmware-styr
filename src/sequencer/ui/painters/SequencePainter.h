/**
 * @file SequencePainter.h
 * @brief Declares the SequencePainter component used by the sequencer UI painter.
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

#include "Config.h"

#include "core/gfx/Canvas.h"

class SequencePainter {
public:
    static void drawLoopStart(Canvas &canvas, int x, int y, int w);
    static void drawLoopEnd(Canvas &canvas, int x, int y, int w);

    static void drawOffset(Canvas &canvas, int baseColor, int x, int y, int w, int h, int offset, int minOffset, int maxOffset);
    static void drawRetrigger(Canvas &canvas, int baseColor, int x, int y, int w, int h, int retrigger, int maxRetrigger);
    static void drawProbability(Canvas &canvas, int baseColor, int x, int y, int w, int h, int probability, int maxProbability);
    static void drawLength(Canvas &canvas, int baseColor, int x, int y, int w, int h, int length, int maxLength);
    static void drawLengthRange(Canvas &canvas, int baseColor, int x, int y, int w, int h, int length, int range, int maxLength);
    static void drawSlide(Canvas &canvas, int baseColor, int x, int y, int w, int h, bool active);

    static void drawSequenceProgress(Canvas &canvas, int baseColor, int x, int y, int w, int h, float progress);
};
