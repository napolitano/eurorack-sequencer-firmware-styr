/**
 * @file Timer.h
 * @brief Declares the Timer component used by the STM32 sequencer driver.
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

#include <functional>

class Timer {
public:
    enum HardwareTimer {
        // HardwareTimer1,
        // HardwareTimer2,
        HardwareTimer3,
        HardwareTimer4,
        HardwareTimer5,
        // HardwareTimer6,
        HardwareTimerCount,
    };

    Timer(HardwareTimer hardwareTimer);

    void init();

    void reset();
    void enable();
    void disable();

    void setPeriod(uint32_t us);

    void setHandler(std::function<void()> handler);

private:
    HardwareTimer _hardwareTimer;
    std::function<void()> _handler;
};
