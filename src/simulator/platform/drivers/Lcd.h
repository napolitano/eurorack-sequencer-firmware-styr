/**
 * @file Lcd.h
 * @brief Declares the Lcd component used by the simulator driver.
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

#include "core/Simulator.h"
#include "core/FrameBuffer.h"

#include "SystemConfig.h"

#include <cstdint>
#include <cstring>

/**
 * @brief Provides the lcd hardware/platform abstraction.
 */
class Lcd {
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
     * @brief Constructs a Lcd instance.
     */
    Lcd() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {}

    /**
     * @brief Initializes the Lcd and its runtime resources.
     */
    void init() {}

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    void draw(uint8_t *frameBuffer) {
        std::memcpy(_frameBuffer.data(), frameBuffer, _frameBuffer.size());
        _simulator.writeLcd(_frameBuffer);
    }

private:
    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    sim::FrameBuffer _frameBuffer; ///< Simulated LCD framebuffer updated by firmware drawing operations.
};
