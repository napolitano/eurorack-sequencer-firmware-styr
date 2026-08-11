/**
 * @file ClockTimer.h
 * @brief Declares the ClockTimer component used by the STM32 sequencer driver.
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

class ClockTimer {
public:
    struct Listener {
        virtual void onClockTimerTick() = 0;
    };

    void init();

    void reset();
    void enable();
    void disable();

    uint32_t period() const { return _period; }
    void setPeriod(uint32_t us);

    void setListener(Listener *listener);

private:
    uint32_t _period = 0;
};
