/**
 * @file UsbMidi.h
 * @brief Declares the UsbMidi component used by the STM32 sequencer driver.
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

#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include <functional>

#include <cstdint>

/**
 * @brief Provides the usb midi hardware/platform abstraction.
 */
class UsbMidi {
public:
    typedef std::function<void(uint16_t vendorId, uint16_t productId)> ConnectHandler;
    typedef std::function<void()> DisconnectHandler;
    typedef std::function<bool(uint8_t)> RecvFilter;

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
        if (_txQueue.full()) {
            return false;
        }
        _txQueue.write({ cable, message });
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
        if (_rxQueue.empty()) {
            return false;
        }
        auto cableAndMessage = _rxQueue.read();
        *cable = cableAndMessage.cable;
        *message = cableAndMessage.message;
        return true;
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
     * @brief Establishes the requested connection.
     *
     * @param[in] vendorId USB vendor identifier (VID).
     * @param[in] productId USB product identifier (PID).
     */
    void connect(uint16_t vendorId, uint16_t productId) {
        if (_connectHandler) {
            _connectHandler(vendorId, productId);
        }
    }

    /**
     * @brief Closes the active connection.
     */
    void disconnect() {
        if (_disconnectHandler) {
            _disconnectHandler();
        }
    }

    /**
     * @brief Queues a MIDI/frontend message for later delivery.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    void enqueueMessage(uint8_t cable, const MidiMessage &message) {
        if (_rxQueue.full()) {
            // overflow
            ++_rxOverflow;
        }
        _rxQueue.write({ cable, message });
    }

    /**
     * @brief Queues raw message/data bytes for later delivery.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] data Input data to read, decode, copy, or process.
     */
    void enqueueData(uint8_t cable, uint8_t data) {
        if (_recvFilter && !_recvFilter(data)) {
            // _recvFilter(data);
        }
    }

    /**
     * @brief Returns dequeue message.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if dequeue message; otherwise `false`.
     */
    bool dequeueMessage(uint8_t *cable, MidiMessage *message) {
        if (_txQueue.empty()) {
            return false;
        }
        auto messageAndCable = _txQueue.readAndReplace();
        *cable = messageAndCable.cable;
        *message = messageAndCable.message;
        return true;
    }

    ConnectHandler _connectHandler; ///< Callback invoked when a connection is established.
    DisconnectHandler _disconnectHandler; ///< Callback invoked when a connection is closed.
    RecvFilter _recvFilter; ///< Receive filter applied to incoming messages/events.

    /**
     * @brief Provides the cable and message hardware/platform abstraction.
     */
    struct CableAndMessage {
        /**
         * @brief Driver value representing cable.
         */
        uint8_t cable; ///< USB-MIDI cable number associated with the queued packet or event.
        MidiMessage message; ///< MIDI message stored/queued by this state.
    };

    RingBuffer<CableAndMessage, 128> _txQueue; ///< Fixed-capacity FIFO holding pending transmit queue.
    RingBuffer<CableAndMessage, 16> _rxQueue; ///< Fixed-capacity FIFO holding pending receive queue.
    /**
     * @brief Driver value representing rx overflow.
     */
    volatile uint32_t _rxOverflow = 0; ///< Count of receive-queue overflows observed since the driver was initialized.

    friend class UsbH;
};
