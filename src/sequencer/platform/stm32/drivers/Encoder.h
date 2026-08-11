/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the STM32 sequencer driver.
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

#include "core/utils/RingBuffer.h"
#include "core/utils/Debouncer.h"

#include <cstdint>

class Encoder {
public:
    enum Event {
        Left,   // encoder turned left
        Right,  // encoder turned right
        Down,   // encoder pressed
        Up,     // encoder released
    };

    Encoder(bool reverse = false);

    void init();

    void process();

    inline bool nextEvent(Event &event) {
        if (_events.readable() < 1) {
            return false;
        }
        event = Event(_events.read());
        return true;
    }

private:
    bool _reverse;

    RingBuffer<uint8_t, 32> _events;

    Debouncer<3> _switchDebouncer;
    bool _switchState = false;
    uint8_t _encoderState = 0;
};
