/**
 * @file MidiConfig.h
 * @brief Declares the MidiConfig component used by the simulator frontend.
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

#include <cstdint>

namespace sim {

/**
 * @brief Stores and edits midi config configuration.
 */
struct MidiConfig {
    /**
     * @brief Pointer to port in; `nullptr` denotes that no object/resource is assigned.
     */
    const char *portIn; ///< Pointer to port in; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Pointer to port out; `nullptr` denotes that no object/resource is assigned.
     */
    const char *portOut; ///< Pointer to port out; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief USB vendor identifier.
     */
    uint16_t vendorId; ///< USB vendor identifier.
    /**
     * @brief USB product identifier.
     */
    uint16_t productId; ///< USB product identifier.
};

static const MidiConfig midiPortConfig = {
    .portIn = "Launchkey Mini LK Mini MIDI",
    .portOut = "Launchkey Mini LK Mini MIDI"
};

static const MidiConfig usbMidiPortConfig = {
    .portIn = "Launchpad Mini 2",
    .portOut = "Launchpad Mini 2",
    .vendorId = 0x1235,
    .productId = 0x0037
};

} // namespace sim
