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

/**
 * @brief Renders sequence state to the display canvas.
 */
class SequencePainter {
public:
    /**
     * @brief Draws loop start.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    static void drawLoopStart(Canvas &canvas, int x, int y, int w);
    /**
     * @brief Draws loop end.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    static void drawLoopEnd(Canvas &canvas, int x, int y, int w);

    /**
     * @brief Draws offset.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] offset Offset applied in the domain defined by the operation.
     * @param[in] minOffset Minimum microtiming/gate offset represented by the painter.
     * @param[in] maxOffset Maximum microtiming/gate offset represented by the painter.
     */
    static void drawOffset(Canvas &canvas, int baseColor, int x, int y, int w, int h, int offset, int minOffset, int maxOffset);
    /**
     * @brief Draws retrigger.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] retrigger Retrigger count/value represented by the painter.
     * @param[in] maxRetrigger Maximum retrigger count used to normalize the display.
     */
    static void drawRetrigger(Canvas &canvas, int baseColor, int x, int y, int w, int h, int retrigger, int maxRetrigger);
    /**
     * @brief Draws probability.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] probability Probability value in the range defined by the model.
     * @param[in] maxProbability Maximum probability value used to normalize the display.
     */
    static void drawProbability(Canvas &canvas, int baseColor, int x, int y, int w, int h, int probability, int maxProbability);
    /**
     * @brief Draws length.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] length Number of bytes or elements covered by the operation.
     * @param[in] maxLength Maximum permitted string/buffer length.
     */
    static void drawLength(Canvas &canvas, int baseColor, int x, int y, int w, int h, int length, int maxLength);
    /**
     * @brief Draws length range.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] length Number of bytes or elements covered by the operation.
     * @param[in] range Inclusive/exclusive value span as defined by the owning API.
     * @param[in] maxLength Maximum permitted string/buffer length.
     */
    static void drawLengthRange(Canvas &canvas, int baseColor, int x, int y, int w, int h, int length, int range, int maxLength);
    /**
     * @brief Draws slide.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] active Whether the addressed state should be active.
     */
    static void drawSlide(Canvas &canvas, int baseColor, int x, int y, int w, int h, bool active);

    /**
     * @brief Draws sequence progress.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] baseColor Base color from which the rendered/output color is derived.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] progress Normalized or discrete progress value used by the UI.
     */
    static void drawSequenceProgress(Canvas &canvas, int baseColor, int x, int y, int w, int h, float progress);
};
