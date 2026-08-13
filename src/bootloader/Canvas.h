/**
 * @file Canvas.h
 * @brief Declares the Canvas component used by the bootloader.
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

#include <algorithm>

#include <cstdint>

/**
 * @brief Provides primitive drawing, clipping, bitmap and text rendering on a framebuffer.
 */
class Canvas {
public:
    /**
     * @brief Width constant used by this component.
     */
    static constexpr int Width = CONFIG_LCD_WIDTH; ///< Display/framebuffer width in pixels.
    /**
     * @brief Height constant used by this component.
     */
    static constexpr int Height = CONFIG_LCD_HEIGHT; ///< Display/framebuffer height in pixels.

    /**
     * @brief Makes the page, dialog, or message visible.
     */
    static void show();

    /**
     * @brief Returns the color.
     *
     * @return Current drawing color.
     */
    static uint8_t color() { return _color; }
    /**
     * @brief Sets the color.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    static void setColor(uint8_t color) { _color = color; }

    /**
     * @brief Fills the target region or buffer using the current/supplied value.
     */
    static void fill();

    /**
     * @brief Draws a single point.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     */
    static void point(int x, int y);

    /**
     * @brief Draws a horizontal line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    static void hline(int x, int y, int w);
    /**
     * @brief Draws a vertical line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    static void vline(int x, int y, int h);

    /**
     * @brief Draws a rectangle outline.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    static void drawRect(int x, int y, int w, int h);
    /**
     * @brief Fills a rectangular region.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    static void fillRect(int x, int y, int w, int h);

    /**
     * @brief Draws a packed 1-bit bitmap.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] bitmap Pointer to packed bitmap data in the format expected by the drawing method.
     */
    static void drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap);
    /**
     * @brief Draws a text string at the requested position.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] str Str consumed by `drawText()`.
     */
    static void drawText(int x, int y, const char *str);
    /**
     * @brief Measures the rendered width of a text string.
     *
     * @param[in] str Str consumed by `textWidth()`.
     *
     * @return Rendered text width in pixels.
     */
    static int textWidth(const char *str);

private:
    /**
     * @brief Clips a horizontal span to the active drawing bounds.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     */
    static void hclip(int &x) {
        x = std::max(0, std::min(Width - 1, x));
    }

    /**
     * @brief Clips a vertical span to the active drawing bounds.
     *
     * @param[in] y Vertical coordinate or scalar y component.
     */
    static void vclip(int &y) {
        y = std::max(0, std::min(Width - 1, y));
    }

    /**
     * @brief Clips the supplied geometry to the active bounds.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     */
    static void clip(int &x, int &y) {
        hclip(x);
        vclip(y);
    }

    /**
     * @brief Reports whether hinside.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     *
     * @return `true` if hinside; otherwise `false`.
     */
    static bool hinside(int x) {
        return x >= 0 && x <= Width - 1;
    }

    /**
     * @brief Reports whether vinside.
     *
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return `true` if vinside; otherwise `false`.
     */
    static bool vinside(int y) {
        return y >= 0 && y <= Height - 1;
    }

    /**
     * @brief Reports whether inside.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return `true` if inside; otherwise `false`.
     */
    static bool inside(int x, int y) {
        return hinside(x) && vinside(y);
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    static void write(int x, int y, uint8_t c) {
        _frameBuffer[y * Width + x] = c;
    }

    /**
     * @brief Bootloader value representing frame buffer.
     */
    static uint8_t _frameBuffer[Width * Height]; ///< Packed display framebuffer owned by the display driver.
    /**
     * @brief Current drawing color/index.
     */
    static uint8_t _color; ///< Current drawing color/index.
};
