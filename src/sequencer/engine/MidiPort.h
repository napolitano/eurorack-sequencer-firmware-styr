/**
 * @file MidiPort.h
 * @brief Declares the MidiPort component used by the sequencer engine.
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

#include "Config.h"
#include <cstdint>

enum class MidiPort : uint8_t {
    Midi,
    UsbMidi,
    CvGate,
};

static const char *midiPortName(MidiPort port) {
    switch (port) {
    case MidiPort::Midi:    return TXT_LIST_LABEL_MIDI;
    case MidiPort::UsbMidi: return TXT_LIST_LABEL_USB;
    case MidiPort::CvGate:  return TXT_LIST_LABEL_CV_GATE;
    }
    return nullptr;
}
