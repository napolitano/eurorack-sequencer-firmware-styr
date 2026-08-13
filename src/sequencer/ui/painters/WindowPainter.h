/**
 * @file WindowPainter.h
 * @brief Declares the WindowPainter component used by the sequencer UI painter.
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

#include "model/Model.h"

#include "engine/Engine.h"

#include "ui/Key.h"

#include "core/gfx/Canvas.h"

/**
 * @brief Renders window state to the display canvas.
 */
class WindowPainter {
public:
    /**
     * @brief Clears stored runtime/container state.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    static void clear(Canvas &canvas);

    /**
     * @brief Draws frame.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    static void drawFrame(Canvas &canvas, int x, int y, int w, int h);

    /**
     * @brief Draws function keys.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] names Collection/table of display names.
     * @param[in] keyState Current key/button state used by the UI operation.
     * @param[in] highlight Highlight state or intensity used for rendering.
     */
    static void drawFunctionKeys(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight = -1);

    /**
     * @brief Draws clock.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] engine Sequencer engine that owns or drives this component.
     */
    static void drawClock(Canvas &canvas, const Engine &engine);

    /**
     * @brief Draws active state.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] track Track model operated on by this component.
     * @param[in] playPattern Pattern currently used for playback.
     * @param[in] editPattern Pattern currently selected for editing.
     * @param[in] snapshotActive Whether snapshot active is enabled for this operation.
     * @param[in] songActive Whether song active is enabled for this operation.
     */
    static void drawActiveState(Canvas &canvas, const Engine &engine, int track, int playPattern, int editPattern, bool snapshotActive, bool songActive);

    /**
     * @brief Draws active mode.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] mode Mode to select.
     */
    static void drawActiveMode(Canvas &canvas, const char *mode);
    /**
     * @brief Draws active function.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] function Function selector or callback consumed by the operation.
     */
    static void drawActiveFunction(Canvas &canvas, const char *function);

    /**
     * @brief Draws header.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] mode Mode to select.
     */
    static void drawHeader(Canvas &canvas, const Model &model, const Engine &engine, const char *mode);
    /**
     * @brief Draws footer.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    static void drawFooter(Canvas &canvas);
    /**
     * @brief Draws footer.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] names Collection/table of display names.
     * @param[in] keyState Current key/button state used by the UI operation.
     * @param[in] highlight Highlight state or intensity used for rendering.
     */
    static void drawFooter(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight = -1);

    /**
     * @brief Draws scrollbar.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] totalRows Total number of rows represented by the list.
     * @param[in] visibleRows Number of rows that fit in the visible viewport.
     * @param[in] displayRow Zero-based first/active row in the displayed viewport.
     */
    static void drawScrollbar(Canvas &canvas, int totalRows, int visibleRows, int displayRow);

private:
    /**
     * @brief UI value representing page width.
     */
    const static int PageWidth = CONFIG_LCD_WIDTH; ///< UI value representing page width.
    /**
     * @brief UI value representing page height.
     */
    const static int PageHeight = CONFIG_LCD_HEIGHT; ///< UI value representing page height.
    /**
     * @brief UI value representing header height.
     */
    const static int HeaderHeight = 9; ///< UI value representing header height.
    /**
     * @brief UI value representing footer height.
     */
    const static int FooterHeight = 9; ///< UI value representing footer height.
    /**
     * @brief Number of function key items currently tracked or supported.
     */
    const static int FunctionKeyCount = CONFIG_FUNCTION_KEY_COUNT; ///< Number of function key items currently tracked or supported.
    /**
     * @brief UI value representing function key width.
     */
    const static int FunctionKeyWidth = 40; ///< UI value representing function key width.
    /**
     * @brief UI value representing function key height.
     */
    const static int FunctionKeyHeight = 8; ///< UI value representing function key height.
};
