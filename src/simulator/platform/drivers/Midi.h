/**
 * @file Midi.h
 * @brief Declares the Midi component used by the simulator driver.
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

#include "core/midi/MidiMessage.h"

#include "core/Simulator.h"

#include <functional>
#include <deque>

#include <cstdint>

/**
 * @brief Provides the midi hardware/platform abstraction.
 */
class Midi : private sim::TargetInputHandler {
public:
    typedef std::function<bool(uint8_t)> RecvFilter;

    /**
     * @brief Constructs a Midi instance.
     */
    Midi() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    /**
     * @brief Destroys the Midi instance.
     */
    ~Midi() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the Midi and its runtime resources.
     */
    void init() {}

    /**
     * @brief Returns send.
     *
     * @param[in] message Message to process.
     *
     * @return `true` if send; otherwise `false`.
     */
    bool send(const MidiMessage &message) {
        _simulator.writeMidiOutput(sim::MidiEvent::makeMessage(0, message));
        return true;
    }

    /**
     * @brief Returns recv.
     *
     * @param[in] message Message to process.
     *
     * @return `true` if recv; otherwise `false`.
     */
    bool recv(MidiMessage *message) {
        if (!_recvQueue.empty()) {
            *message = _recvQueue.front();
            _recvQueue.pop_front();
            return true;
        }
        return false;
    }

    /**
     * @brief Sets the recv filter.
     *
     * @param[in] filter Receive/filter predicate applied before an event is accepted.
     */
    void setRecvFilter(RecvFilter filter) {
        _recvFilter = filter;
    }

    /**
     * @brief Returns the rx overflow.
     *
     * @return Number of receive messages/events dropped because the input queue overflowed.
     */
    uint32_t rxOverflow() const { return 0; }

private:
    /**
     * @brief Writes midi input.
     *
     * @param[in] event Event to process.
     */
    void writeMidiInput(sim::MidiEvent event) {
        if (event.port == 0 && event.kind == sim::MidiEvent::Message) {
            if (event.message.length() != 1 || !_recvFilter || !_recvFilter(event.message.status())) {
                _recvQueue.emplace_back(event.message);
            }
        }
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    std::deque<MidiMessage> _recvQueue; ///< MIDI messages queued by the simulator frontend for firmware-side receive calls.
    RecvFilter _recvFilter; ///< Receive filter applied to incoming messages/events.
};
