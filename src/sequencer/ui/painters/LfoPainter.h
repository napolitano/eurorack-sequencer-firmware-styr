/**
 * @file LfoPainter.h
 * @brief Declares the LfoPainter component used by the sequencer UI painter.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "core/gfx/Canvas.h"
#include "model/LfoTrack.h"
#include <vector>

class LfoPainter {
public:
    // optional randSteps: if provided and non-null, Random waveforms will use these
    // values for an exact preview that matches the engine's generated table.
    static void drawWaveform(Canvas &canvas, const LfoTrack &lfoTrack, int x, int y, int w, int h, float progress = -1.f, const std::vector<float> *randSteps = nullptr);
};

