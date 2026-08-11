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

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    void init();

    void draw(uint8_t *frameBuffer);

private:
    void sendCmd(uint8_t cmd);
    void sendData(uint8_t data);

    void initialize();

    void setColAddr(uint8_t a, uint8_t b);
    void setRowAddr(uint8_t a, uint8_t b);
    void setWrite();

    uint32_t _frameBuffer[Width * Height / 8];
};
