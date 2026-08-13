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

/**
 * @brief Renders song state to the display canvas.
 */
class SongPainter {
public:
    /**
     * @brief Draws arrow down.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    static void drawArrowDown(Canvas &canvas, int x, int y, int w);
    /**
     * @brief Draws arrow up.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    static void drawArrowUp(Canvas &canvas, int x, int y, int w);
    /**
     * @brief Draws arrow right.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    static void drawArrowRight(Canvas &canvas, int x, int y, int w, int h);
    /**
     * @brief Draws progress.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] progress Normalized or discrete progress value used by the UI.
     */
    static void drawProgress(Canvas &canvas, int x, int y, int w, int h, float progress);
};
