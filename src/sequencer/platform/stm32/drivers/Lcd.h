/**
 * @file Lcd.h
 * @brief Declares the Lcd component used by the STM32 sequencer driver.
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

#include "SystemConfig.h"

#include <cstdint>
#include <cstdlib>

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
     * @brief Initializes the Lcd and its runtime resources.
     */
    void init();

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    void draw(uint8_t *frameBuffer);

private:
    /**
     * @brief Sends cmd.
     *
     * @param[in] cmd Command byte/value sent to the target interface.
     */
    void sendCmd(uint8_t cmd);
    /**
     * @brief Sends data.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     */
    void sendData(uint8_t data);

    /**
     * @brief Initializes the Lcd state.
     */
    void initialize();

    /**
     * @brief Sets the col addr.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     * @param[in] b Right/second value used by the comparison or binary operation.
     */
    void setColAddr(uint8_t a, uint8_t b);
    /**
     * @brief Sets the row addr.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     * @param[in] b Right/second value used by the comparison or binary operation.
     */
    void setRowAddr(uint8_t a, uint8_t b);
    /**
     * @brief Sets the write.
     */
    void setWrite();

    /**
     * @brief Driver value representing frame buffer.
     */
    uint32_t _frameBuffer[Width * Height / 8]; ///< Packed display framebuffer owned by the display driver.
};
