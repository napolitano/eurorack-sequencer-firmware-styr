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

/**
 * @brief Provides the encoder hardware/platform abstraction.
 */
class Encoder : private sim::TargetInputHandler {
public:
    /**
     * @brief Enumerates the supported event values.
     */
    enum Event {
        Left,   ///< Encoder rotation toward decreasing values.
        Right,  ///< Encoder rotation toward increasing values.
        Down,   ///< Encoder push switch transitioned to pressed.
        Up,     ///< Encoder push switch transitioned to released.
    };

    /**
     * @brief Constructs a Encoder instance.
     */
    Encoder() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    /**
     * @brief Destroys the Encoder instance.
     */
    ~Encoder() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the Encoder and its runtime resources.
     */
    void init() {}

    /**
     * @brief Processes the supplied input for this component.
     */
    void process() {}

    /**
     * @brief Returns next event.
     *
     * @param[in] event Event to process.
     *
     * @return `true` if next event; otherwise `false`.
     */
    bool nextEvent(Event &event) {
        if (_events.empty()) {
            return false;
        }
        event = _events.front();
        _events.pop_front();
        return true;
    }

private:
    /**
     * @brief Writes encoder.
     *
     * @param[in] event Event to process.
     */
    void writeEncoder(sim::EncoderEvent event) override {
        switch (event) {
        case sim::EncoderEvent::Left:   _events.emplace_back(Left);     break;
        case sim::EncoderEvent::Right:  _events.emplace_back(Right);    break;
        case sim::EncoderEvent::Up:     _events.emplace_back(Up);       break;
        case sim::EncoderEvent::Down:   _events.emplace_back(Down);     break;
        }
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    std::deque<Event> _events; ///< Queued simulated encoder rotations/button transitions waiting to be consumed by the driver.
};
