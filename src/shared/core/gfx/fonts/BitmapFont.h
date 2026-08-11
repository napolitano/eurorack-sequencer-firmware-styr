/**
 * @file BitmapFont.h
 * @brief Declares the BitmapFont component used by the shared graphics support.
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
#ifndef __BITMAP_FONT_H__
#define __BITMAP_FONT_H__

#include <stdint.h>

// Glpyh data
typedef struct {
    uint16_t offset;            // Offset in bitmap array
    uint8_t width, height;      // Bitmap dimensions in pixels
    uint8_t xAdvance;           // Distance to advance cursor (x axis)
    int8_t xOffset, yOffset;    // Distance from cursor to upper-left corner
} BitmapFontGlyph;

// Font data
typedef struct {
    uint8_t bpp;                // Bits per pixel
    uint8_t *bitmap;            // Glyph bitmaps, concatenated
    BitmapFontGlyph *glyphs;    // Glyph array
    uint8_t first, last;        // ASCII extents
    uint8_t yAdvance;           // Newline distance (y axis)
} BitmapFont;

#endif // __BITMAP_FONT_H__
