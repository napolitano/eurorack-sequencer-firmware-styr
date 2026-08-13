/**
 * @file UsbMidi.h
 * @brief Declares the UsbMidi component used by the simulator driver.
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
#include <memory>

#include <cstdint>

/**
 * @brief Provides the usb midi hardware/platform abstraction.
 */
class UsbMidi : private sim::TargetInputHandler {
public:
    typedef std::function<void(uint16_t vendorId, uint16_t productId)> ConnectHandler;
    typedef std::function<void()> DisconnectHandler;
    typedef std::function<bool(uint8_t)> RecvFilter;

    /**
     * @brief Constructs a UsbMidi instance.
     */
    UsbMidi() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {
        _simulator.registerTargetInputObserver(this);
    }

    /**
     * @brief Destroys the UsbMidi instance.
     */
    ~UsbMidi() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the UsbMidi and its runtime resources.
     */
    void init() {}

    /**
     * @brief Returns send.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if send; otherwise `false`.
     */
    bool send(uint8_t cable, const MidiMessage &message) {
        _simulator.writeMidiOutput(sim::MidiEvent::makeMessage(1, message));
        return true;
    }

    /**
     * @brief Returns recv.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if recv; otherwise `false`.
     */
    bool recv(uint8_t *cable, MidiMessage *message) {
        if (!_recvQueue.empty()) {
            *cable = 0;
            *message = _recvQueue.front();
            _recvQueue.pop_front();
            return true;
        }
        return false;
    }

    /**
     * @brief Sets the connect handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setConnectHandler(ConnectHandler handler) {
        _connectHandler = handler;
    }

    /**
     * @brief Sets the disconnect handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setDisconnectHandler(DisconnectHandler handler) {
        _disconnectHandler = handler;
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
        if (event.port == 1) {
            switch (event.kind) {
            case sim::MidiEvent::Connect:
                if (_connectHandler) {
                    _connectHandler(event.connect.vendorId, event.connect.productId);
                }
                break;
            case sim::MidiEvent::Disconnect:
                if (_disconnectHandler) {
                    _disconnectHandler();
                }
                break;
            case sim::MidiEvent::Message:
                if (event.message.length() != 1 || !_recvFilter || !_recvFilter(event.message.status())) {
                    _recvQueue.emplace_back(event.message);
                }
                break;
            }
        }
    }

    ConnectHandler _connectHandler; ///< Callback invoked when a connection is established.
    DisconnectHandler _disconnectHandler; ///< Callback invoked when a connection is closed.
    RecvFilter _recvFilter; ///< Receive filter applied to incoming messages/events.

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    std::deque<MidiMessage> _recvQueue; ///< USB-MIDI messages queued by the simulator frontend for firmware-side receive calls.
};
