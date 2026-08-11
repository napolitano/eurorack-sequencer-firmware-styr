/**
 * @file Blit.h
 * @brief Declares the Blit component used by the shared graphics support.
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

#include <cstdint>

namespace blit {
    struct set {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            frameBuffer(x, y) = color;
        }
    };
    struct add {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::min(0xff, int(frameBuffer(x, y)) + color);
            frameBuffer(x, y) += color;
        }
    };
    struct sub {
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::max(0x00, int(frameBuffer(x, y)) - color);
            frameBuffer(x, y) -= std::min(frameBuffer(x, y), color);
        }
    };
};
