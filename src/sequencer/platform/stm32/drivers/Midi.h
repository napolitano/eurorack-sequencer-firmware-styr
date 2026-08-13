/**
 * @file Midi.h
 * @brief Declares the Midi component used by the STM32 sequencer driver.
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
#include "core/midi/MidiParser.h"
#include "core/utils/RingBuffer.h"

#include <functional>

#include <cstdint>

/**
 * @brief Provides the midi hardware/platform abstraction.
 */
class Midi {
public:
    typedef std::function<bool(uint8_t)> RecvFilter;

    /**
     * @brief Initializes the Midi and its runtime resources.
     */
    void init();

    /**
     * @brief Returns send.
     *
     * @param[in] message Message to process.
     *
     * @return `true` if send; otherwise `false`.
     */
    bool send(const MidiMessage &message);
    /**
     * @brief Returns recv.
     *
     * @param[in] message Message to process.
     *
     * @return `true` if recv; otherwise `false`.
     */
    bool recv(MidiMessage *message);

    /**
     * @brief Sets the recv filter.
     *
     * @param[in] filter Receive/filter predicate applied before an event is accepted.
     */
    void setRecvFilter(RecvFilter filter);

    /**
     * @brief Returns the rx overflow.
     *
     * @return Number of receive messages/events dropped because the input queue overflowed.
     */
    uint32_t rxOverflow() const { return _rxOverflow; }

    /**
     * @brief Handles irq.
     */
    void handleIrq();
private:
    /**
     * @brief Sends the supplied data through this transport/interface.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     */
    void send(uint8_t data);

    RingBuffer<uint8_t, 64> _txBuffer; ///< Fixed-capacity FIFO holding pending transmit buffer.
    RingBuffer<uint8_t, 64> _rxBuffer; ///< Fixed-capacity FIFO holding pending receive buffer.
    /**
     * @brief Driver value representing rx overflow.
     */
    volatile uint32_t _rxOverflow = 0; ///< Count of receive-queue overflows observed since the driver was initialized.
    /**
     * @brief Whether tx is currently active.
     */
    volatile uint32_t _txActive = 0; ///< Whether tx is currently active.

    RecvFilter _recvFilter; ///< Receive filter applied to incoming messages/events.
    MidiParser _midiParser; ///< Byte-stream MIDI parser used to reconstruct messages from the UART receive buffer.
};
