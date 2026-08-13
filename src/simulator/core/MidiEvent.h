/**
 * @file MidiEvent.h
 * @brief Declares the MidiEvent component used by the simulator core.
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

#include <cstdint>

namespace sim {

/**
 * @brief Provides midi event behavior for the desktop simulator.
 */
struct MidiEvent {
    /**
     * @brief Enumerates the supported kind values.
     */
    enum Kind {
        Connect, ///< Selects the connect kind.
        Disconnect, ///< Selects the disconnect kind.
        Message, ///< Selects the message kind.
    };

    /**
     * @brief Simulator value representing kind.
     */
    int kind; ///< MIDI event kind identifying how the remaining fields are interpreted.
    /**
     * @brief Simulator value representing port.
     */
    int port; ///< GPIO peripheral base address used to drive the debug LED.
    MidiMessage message; ///< MIDI message stored/queued by this state.
    /**
     * @brief USB vendor identifier.
     */
    struct {
        uint16_t vendorId;
        uint16_t productId;
    } connect;

    /**
     * @brief Constructs a MidiEvent instance.
     */
    MidiEvent() : message() {}
    /**
     * @brief Constructs a MidiEvent instance.
     *
     * @param[in] kind MIDI event kind used to interpret the event payload.
     * @param[in] port MIDI or logical port selected for the operation.
     */
    MidiEvent(Kind kind, int port) : kind(kind), port(port) {}
    /**
     * @brief Constructs a MidiEvent instance.
     *
     * @param[in] other Other object/value used for comparison or combination.
     */
    MidiEvent(const MidiEvent &other) = default;

    /**
     * @brief Copies all MIDI event fields from another event.
     *
     * @param[in] other MIDI event to copy.
     *
     * @return Reference to this event after assignment.
     */
    /**
     * @brief Copies all MIDI event fields from another event.
     *
     * @param[in] other MIDI event to copy.
     *
     * @return Reference to this event after assignment.
     */
    MidiEvent &operator=(const MidiEvent &other) = default;

    /**
     * @brief Returns make connect.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] vendorId USB vendor identifier (VID).
     * @param[in] productId USB product identifier (PID).
     *
     * @return Result of makeConnect().
     */
    static MidiEvent makeConnect(int port, uint16_t vendorId, uint16_t productId) {
        MidiEvent event(Connect, port);
        event.connect = { vendorId, productId };
        return event;
    }

    /**
     * @brief Returns make disconnect.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     *
     * @return Result of makeDisconnect().
     */
    static MidiEvent makeDisconnect(int port) {
        MidiEvent event(Disconnect, port);
        return event;
    }

    /**
     * @brief Returns make message.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     *
     * @return Result of makeMessage().
     */
    static MidiEvent makeMessage(int port, MidiMessage message) {
        MidiEvent event(Message, port);
        event.message = message;
        return event;
    }
};

} // namespace sim
