/**
 * @file MidiUtils.h
 * @brief Declares the MidiUtils component used by the sequencer engine.
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

#include "MidiPort.h"

#include "model/MidiConfig.h"

#include "core/midi/MidiMessage.h"

namespace MidiUtils {

/**
 * @brief Reports whether a MIDI message matches a configured MIDI source.
 *
 * @param[in] port MIDI transport port associated with the message/source.
 * @param[in] message MIDI message to test against the source configuration.
 * @param[in] source Configured MIDI source containing port/channel matching rules.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
static bool matchSource(MidiPort port, const MidiMessage &message, const MidiSourceConfig &source) {
    return port == MidiPort(source.port()) && (source.isOmni() || message.channel() == source.channel());
}

} // namespace MidiUtils
