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

/**
 * @brief Renders lfo state to the display canvas.
 */
class LfoPainter {
public:
    // optional randSteps: if provided and non-null, Random waveforms will use these
    // values for an exact preview that matches the engine's generated table.
    /**
     * @brief Draws waveform.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] lfoTrack LFO track model whose state is rendered.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] progress Normalized or discrete progress value used by the UI.
     * @param[in] randSteps Random-step values rendered in the LFO preview.
     */
    static void drawWaveform(Canvas &canvas, const LfoTrack &lfoTrack, int x, int y, int w, int h, float progress = -1.f, const std::vector<float> *randSteps = nullptr);
};

