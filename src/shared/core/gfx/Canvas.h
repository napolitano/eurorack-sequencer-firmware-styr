/**
 * @file Canvas.h
 * @brief Declares the Canvas component used by the shared graphics support.
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

#include "FrameBuffer.h"

#include <algorithm>

#include <cmath>
#include <cstdint>

/**
 * @brief Enumerates the supported blend mode values.
 */
enum class BlendMode {
    Set, ///< Selects the set blend mode.
    Add, ///< Selects the add blend mode.
    Sub, ///< Selects the sub blend mode.
};

/**
 * @brief Enumerates the supported font values.
 */
enum class Font {
    Tiny, ///< Selects the tiny font.
    Small, ///< Selects the small font.
    Default = Tiny, ///< Selects the default font.
};

/**
 * @brief Enumerates the supported horizontal align values.
 */
enum class HorizontalAlign {
    Left, ///< Selects the left horizontal align.
    Right, ///< Selects the right horizontal align.
    Center, ///< Selects the center horizontal align.
};

/**
 * @brief Enumerates the supported vertical align values.
 */
enum class VerticalAlign {
    Top, ///< Selects the top vertical align.
    Bottom, ///< Selects the bottom vertical align.
    Center, ///< Selects the center vertical align.
};

/**
 * @brief Provides primitive drawing, clipping, bitmap and text rendering on a framebuffer.
 */
class Canvas {
public:
    /**
     * @brief Constructs a Canvas instance.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    Canvas(FrameBuffer8bit &frameBuffer) :
        /**
         * @brief Returns the frame buffer.
         */
        _frameBuffer(frameBuffer),
        /**
         * @brief Returns the right.
         */
        _right(frameBuffer.width() - 1),
        /**
         * @brief Returns the bottom.
         */
        _bottom(frameBuffer.height() - 1)
    {
    }

    /**
     * @brief Returns the color.
     *
     * @return Current drawing color.
     */
    uint8_t color() const { return _color; }
    /**
     * @brief Sets the color.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setColor(uint8_t color) { _color = color; }

    /**
     * @brief Returns the blend mode.
     *
     * @return Configured blend mode.
     */
    BlendMode blendMode() const { return _blendMode; }
    /**
     * @brief Sets the blend mode.
     *
     * @param[in] blendMode Pixel blending mode used by drawing operations.
     */
    void setBlendMode(BlendMode blendMode) { _blendMode = blendMode; }

    /**
     * @brief Returns the font.
     *
     * @return Font currently selected for text rendering.
     */
    Font font() const { return _font; }
    /**
     * @brief Sets the font.
     *
     * @param[in] font Font selection used for text rendering.
     */
    void setFont(Font font) { _font = font; }

    /**
     * @brief Fills the target region or buffer using the current/supplied value.
     */
    void fill();

    /**
     * @brief Draws a single point.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     */
    void point(int x, int y);

    /**
     * @brief Draws a horizontal line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    void hline(int x, int y, int w);
    /**
     * @brief Draws a vertical line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void vline(int x, int y, int h);

    /**
     * @brief Draws a line between two points.
     *
     * @param[in] x0 Horizontal coordinate of the first point.
     * @param[in] y0 Vertical coordinate of the first point.
     * @param[in] x1 Horizontal coordinate of the second point.
     * @param[in] y1 Vertical coordinate of the second point.
     */
    void line(float x0, float y0, float x1, float y1);

    /**
     * @brief Draws a rectangle outline.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void drawRect(int x, int y, int w, int h);
    /**
     * @brief Fills a rectangular region.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void fillRect(int x, int y, int w, int h);

    /**
     * @brief Draws a packed 1-bit bitmap.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] bitmap Pointer to packed bitmap data in the format expected by the drawing method.
     */
    void drawBitmap1bit(int x, int y, int w, int h, const uint8_t *bitmap);
    /**
     * @brief Draws a packed 4-bit bitmap.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] bitmap Pointer to packed bitmap data in the format expected by the drawing method.
     */
    void drawBitmap4bit(int x, int y, int w, int h, const uint8_t *bitmap);

    /**
     * @brief Draws a text string at the requested position.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] str Str consumed by `drawText()`.
     */
    void drawText(int x, int y, const char *str);
    /**
     * @brief Draws text centered inside the requested rectangle.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] str Str consumed by `drawTextCentered()`.
     */
    void drawTextCentered(int x, int y, int w, int h, const char *str);
    /**
     * @brief Draws text inside a rectangle using explicit horizontal and vertical alignment.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] horizontalAlign Horizontal alignment applied to the rendered text.
     * @param[in] verticalAlign Vertical alignment applied to the rendered text.
     * @param[in] str Str consumed by `drawTextAligned()`.
     */
    void drawTextAligned(int x, int y, int w, int h, HorizontalAlign horizontalAlign, VerticalAlign verticalAlign, const char *str);
    /**
     * @brief Draws wrapped/multiline text in the requested width.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] str Str consumed by `drawTextMultiline()`.
     */
    void drawTextMultiline(int x, int y, int w, const char *str);

    /**
     * @brief Measures the rendered width of a text string.
     *
     * @param[in] str Str consumed by `textWidth()`.
     *
     * @return Rendered text width in pixels.
     */
    int textWidth(const char *str);
    /**
     * @brief Measures the rendered height of a text string.
     *
     * @param[in] str Str consumed by `textHeight()`.
     *
     * @return Rendered text height in pixels.
     */
    int textHeight(const char *str);


private:
    /**
     * @brief Clips a horizontal span to the active drawing bounds.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     */
    void hclip(int &x) {
        x = std::max(0, std::min(_right, x));
    }

    /**
     * @brief Clips a vertical span to the active drawing bounds.
     *
     * @param[in] y Vertical coordinate or scalar y component.
     */
    void vclip(int &y) {
        y = std::max(0, std::min(_bottom, y));
    }

    /**
     * @brief Clips the supplied geometry to the active bounds.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     */
    void clip(int &x, int &y) {
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
    bool hinside(int x) {
        return x >= 0 && x <= _right;
    }

    /**
     * @brief Reports whether vinside.
     *
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return `true` if vinside; otherwise `false`.
     */
    bool vinside(int y) {
        return y >= 0 && y <= _bottom;
    }

    /**
     * @brief Reports whether inside.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return `true` if inside; otherwise `false`.
     */
    bool inside(int x, int y) {
        return hinside(x) && vinside(y);
    }

    template<typename Blit>
    /**
     * @brief Draws a single point.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     */
    void point(int x, int y) {
        Blit blit;
        if (inside(x, y)) {
            blit(_frameBuffer, x, y, _color);
        }
    }

    template<typename Blit>
    /**
     * @brief Draws a horizontal line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    void hline(int x, int y, int w) {
        Blit blit;
        if (vinside(y)) {
            int x0 = x, x1 = x + w - 1;
            hclip(x0);
            hclip(x1);
            for (int x = x0; x <= x1; ++x) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit>
    /**
     * @brief Draws a vertical line.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void vline(int x, int y, int h) {
        Blit blit;
        if (hinside(x)) {
            int y0 = y, y1 = y + h - 1;
            vclip(y0);
            vclip(y1);
            for (int y = y0; y <= y1; ++y) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit>
    /**
     * @brief Draws a line between two points.
     *
     * @param[in] x0 Horizontal coordinate of the first point.
     * @param[in] y0 Vertical coordinate of the first point.
     * @param[in] x1 Horizontal coordinate of the second point.
     * @param[in] y1 Vertical coordinate of the second point.
     */
    void line(float x0, float y0, float x1, float y1) {
        Blit blit;

        auto plot = [&] (int x, int y, float c) {
            if (inside(x, y)) {
                blit(_frameBuffer, x, y, _color * c);
            }
        };

        auto ipart = [] (float x) { return std::floor(x); };
        auto round = [] (float x) { return std::floor(x + 0.5f); };
        auto fpart = [] (float x) { return x - std::floor(x); };
        auto rfpart = [] (float x) { return 1.f - (x - std::floor(x)); };

        bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

        if (steep) {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        float dx = x1 - x0;
        float dy = y1 - y0;
        float gradient = dx == 0.f ? 1.f : dy / dx;

        // first endpoint
        int xend = round(x0);
        float yend = y0 + gradient * (xend - x0);
        float xgap = rfpart(x0 + 0.5f);
        int xpxl1 = xend;
        int ypxl1 = ipart(yend);
        if (steep) {
            plot(ypxl1,     xpxl1, rfpart(yend) * xgap);
            plot(ypxl1 + 1, xpxl1,  fpart(yend) * xgap);
        } else {
            plot(xpxl1, ypxl1,     rfpart(yend) * xgap);
            plot(xpxl1, ypxl1 + 1,  fpart(yend) * xgap);
        }
        float intery = yend + gradient;

        // second endpoint
        xend = round(x1);
        yend = y1 + gradient * (xend - x1);
        xgap = fpart(x1 + 0.5f);
        int xpxl2 = xend;
        int ypxl2 = ipart(yend);
        if (steep) {
            plot(ypxl2,     xpxl2, rfpart(yend) * xgap);
            plot(ypxl2 + 1, xpxl2,  fpart(yend) * xgap);
        } else {
            plot(xpxl2, ypxl2,    rfpart(yend) * xgap);
            plot(xpxl2, ypxl2 + 1, fpart(yend) * xgap);
        }

        // main loop
        if (steep) {
            for (int x = xpxl1 + 1; x < xpxl2; ++x) {
                plot(ipart(intery),     x, rfpart(intery));
                plot(ipart(intery) + 1, x,  fpart(intery));
                intery = intery + gradient;
            }
        } else {
            for (int x = xpxl1 + 1; x < xpxl2; ++x) {
                plot(x, ipart(intery),    rfpart(intery));
                plot(x, ipart(intery) + 1, fpart(intery));
                intery = intery + gradient;
            }
        }
    }

    template<typename Blit>
    /**
     * @brief Draws a rectangle outline.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void drawRect(int x, int y, int w, int h) {
        hline(x, y, w);
        hline(x, y + h - 1, w);
        vline(x, y + 1, h - 2);
        vline(x + w - 1, y + 1, h - 2);
    }

    template<typename Blit>
    /**
     * @brief Fills a rectangular region.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     */
    void fillRect(int x, int y, int w, int h) {
        Blit blit;
        int x0 = x, x1 = x + w - 1;
        int y0 = y, y1 = y + h - 1;
        clip(x0, y0);
        clip(x1, y1);
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                blit(_frameBuffer, x, y, _color);
            }
        }
    }

    template<typename Blit, size_t Bpp>
    /**
     * @brief Draws bitmap.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     * @param[in] h Height in pixels or the coordinate domain used by the caller.
     * @param[in] bitmap Pointer to packed bitmap data in the format expected by the drawing method.
     */
    void drawBitmap(int x, int y, int w, int h, const uint8_t *bitmap) {
        Blit blit;
        int x0 = x, x1 = x + w - 1;
        int y0 = y, y1 = y + h - 1;
        if (x0 > _right || x1 < 0 || y0 > _bottom || y1 < 0) {
            return;
        }

        const uint8_t mask = (1 << Bpp) - 1;
        int shift = 0;
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                // uint8_t pixel = ((*bitmap >> shift) & mask) << (8 - Bpp);
                uint8_t pixel = ((*bitmap >> shift) & mask) * _color;
                shift += Bpp;
                if (shift >= 8) {
                    ++bitmap;
                    shift = 0;
                }
                if (inside(x, y)) {
                    blit(_frameBuffer, x, y, pixel);
                }
            }
        }
    }

    /**
     * @brief Reference to frame buffer owned by another component.
     */
    FrameBuffer8bit &_frameBuffer; ///< Reference to frame buffer owned by another component.
    /**
     * @brief Graphics value representing right.
     */
    int _right; ///< Inclusive/exclusive right clipping boundary in canvas coordinates.
    /**
     * @brief Graphics value representing bottom.
     */
    int _bottom; ///< Inclusive/exclusive bottom clipping boundary in canvas coordinates.
    /**
     * @brief Current drawing color/index.
     */
    uint8_t _color = 0xf; ///< Current drawing color/index.
    BlendMode _blendMode = BlendMode::Set; ///< Active blend mode controlling the behavior of `Canvas`.
    Font _font = Font::Default; ///< Font state used by `Canvas` to preserve its current configuration or runtime progress.
};
