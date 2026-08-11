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

class Lcd {
public:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    Lcd() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void draw(uint8_t *frameBuffer) {
        std::memcpy(_frameBuffer.data(), frameBuffer, _frameBuffer.size());
        _simulator.writeLcd(_frameBuffer);
    }

private:
    sim::Simulator &_simulator;
    sim::FrameBuffer _frameBuffer;
};
