/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the simulator driver.
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

#include "core/Simulator.h"

#include <deque>

class Encoder : private sim::TargetInputHandler {
public:
    enum Event {
        Left,   // encoder turned left
        Right,  // encoder turned right
        Down,   // encoder pressed
        Up,     // encoder released
    };

    Encoder() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    void init() {}

    void process() {}

    bool nextEvent(Event &event) {
        if (_events.empty()) {
            return false;
        }
        event = _events.front();
        _events.pop_front();
        return true;
    }

private:
    void writeEncoder(sim::EncoderEvent event) override {
        switch (event) {
        case sim::EncoderEvent::Left:   _events.emplace_back(Left);     break;
        case sim::EncoderEvent::Right:  _events.emplace_back(Right);    break;
        case sim::EncoderEvent::Up:     _events.emplace_back(Up);       break;
        case sim::EncoderEvent::Down:   _events.emplace_back(Down);     break;
        }
    }

    sim::Simulator &_simulator;
    std::deque<Event> _events;
};
