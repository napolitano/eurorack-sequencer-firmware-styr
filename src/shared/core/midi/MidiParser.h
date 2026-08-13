/**
 * @file MidiParser.h
 * @brief Declares the MidiParser component used by the shared MIDI support.
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

#include "MidiMessage.h"

#include <cstdint>

/**
 * @brief Converts an incoming MIDI byte stream into complete `MidiMessage` objects.
 */
class MidiParser {
public:
    /**
     * @brief Constructs a MidiParser instance.
     */
    MidiParser() {
    }

    /**
     * @brief Returns feed.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     *
     * @return `true` if feed; otherwise `false`.
     */
    bool feed(uint8_t data);

    /**
     * @brief Returns the message.
     *
     * @return Reference to the message.
     */
    const MidiMessage &message() const {
        return _message;
    }

private:
    /**
     * @brief Status, in microseconds.
     */
    uint8_t _status = 0; ///< Running MIDI status byte used while assembling channel messages.
    /**
     * @brief Runtime value representing data.
     */
    uint8_t _data[2] = { 0, 0 }; ///< Buffered payload bytes used while parsing or transporting the current message.
    /**
     * @brief Zero-based data index; a negative/sentinel value represents no selection where applicable.
     */
    uint8_t _dataIndex = 0; ///< Zero-based data index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Runtime value representing data length.
     */
    uint8_t _dataLength = 0; ///< Number of valid payload bytes currently buffered.
    /**
     * @brief Whether recv system exclusive is true in the current state.
     */
    bool _recvSystemExclusive = false; ///< True while the MIDI parser is assembling a System Exclusive message.

    MidiMessage _message; ///< MIDI message stored/queued by this state.
};
