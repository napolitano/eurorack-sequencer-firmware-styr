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
    /**
     * @brief Stores the fields required to represent set.
     */
    struct set {
        /**
         * @brief Implements operator() for this type.
         *
         * @param[in] frameBuffer Framebuffer that receives drawing operations.
         * @param[in] x Horizontal coordinate or scalar x component.
         * @param[in] y Vertical coordinate or scalar y component.
         * @param[in] color Color value/index to draw, store, or emit.
         */
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            frameBuffer(x, y) = color;
        }
    };
    /**
     * @brief Stores the fields required to represent add.
     */
    struct add {
        /**
         * @brief Implements operator() for this type.
         *
         * @param[in] frameBuffer Framebuffer that receives drawing operations.
         * @param[in] x Horizontal coordinate or scalar x component.
         * @param[in] y Vertical coordinate or scalar y component.
         * @param[in] color Color value/index to draw, store, or emit.
         */
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::min(0xff, int(frameBuffer(x, y)) + color);
            frameBuffer(x, y) += color;
        }
    };
    /**
     * @brief Stores the fields required to represent sub.
     */
    struct sub {
        /**
         * @brief Implements operator() for this type.
         *
         * @param[in] frameBuffer Framebuffer that receives drawing operations.
         * @param[in] x Horizontal coordinate or scalar x component.
         * @param[in] y Vertical coordinate or scalar y component.
         * @param[in] color Color value/index to draw, store, or emit.
         */
        void operator()(FrameBuffer8bit &frameBuffer, int x, int y, uint8_t color) {
            // frameBuffer(x, y) = std::max(0x00, int(frameBuffer(x, y)) - color);
            frameBuffer(x, y) -= std::min(frameBuffer(x, y), color);
        }
    };
};
