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

class MidiParser {
public:
    MidiParser() {
    }

    bool feed(uint8_t data);

    const MidiMessage &message() const {
        return _message;
    }

private:
    uint8_t _status = 0;
    uint8_t _data[2] = { 0, 0 };
    uint8_t _dataIndex = 0;
    uint8_t _dataLength = 0;
    bool _recvSystemExclusive = false;

    MidiMessage _message;
};
