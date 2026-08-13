/**
 * @file Midi.h
 * @brief Declares the Midi component used by the simulator frontend.
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

#include "RtMidi.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <deque>
#include <mutex>

#include <cstdint>

namespace sim {

/**
 * @brief Provides midi behavior for the desktop simulator.
 */
class Midi {
public:
    /**
     * @brief Provides port behavior for the desktop simulator.
     */
    class Port {
    public:
        typedef std::function<void(const std::vector<uint8_t> &message)> RecvHandler;
        typedef std::function<void()> ConnectHandler;
        typedef std::function<void()> DisconnectHandler;

        /**
         * @brief Constructs a Port instance.
         *
         * @param[in] portIn Input MIDI/transport port identifier.
         * @param[in] portOut Output MIDI/transport port identifier.
         * @param[in] recvHandler Callback invoked for received transport/MIDI data.
         */
        Port(
            /**
             * @brief Returns the port in.
             *
             * @return Reference to the port in.
             */
            const std::string &portIn,
            /**
             * @brief Returns the port in.
             *
             * @return Reference to the port in.
             */
            const std::string &portOut,
            /**
             * @brief Returns the port in.
             *
             * @return The port in value.
             */
            RecvHandler recvHandler
        /**
         * @brief Returns the port in.
         *
         * @return The port in value.
         */
        ) : _portIn(portIn), _portOut(portOut), _recvHandler(recvHandler) {}

        /**
         * @brief Constructs a Port instance.
         *
         * @param[in] portIn Input MIDI/transport port identifier.
         * @param[in] portOut Output MIDI/transport port identifier.
         * @param[in] recvHandler Callback invoked for received transport/MIDI data.
         * @param[in] connectHandler Callback invoked when a MIDI device/port connects.
         * @param[in] disconnectHandler Callback invoked when a MIDI device/port disconnects.
         */
        Port(
            /**
             * @brief Returns the port in.
             *
             * @return Reference to the port in.
             */
            const std::string &portIn,
            /**
             * @brief Returns the port in.
             *
             * @return Reference to the port in.
             */
            const std::string &portOut,
            /**
             * @brief Returns the port in.
             *
             * @return The port in value.
             */
            RecvHandler recvHandler,
            /**
             * @brief Returns the port in.
             *
             * @return The port in value.
             */
            ConnectHandler connectHandler,
            /**
             * @brief Returns the port in.
             *
             * @return The port in value.
             */
            DisconnectHandler disconnectHandler
        /**
         * @brief Returns the port in.
         *
         * @return The port in value.
         */
        ) : _portIn(portIn), _portOut(portOut), _recvHandler(recvHandler), _connectHandler(connectHandler), _disconnectHandler(disconnectHandler) {}

        /**
         * @brief Reports whether open.
         *
         * @return `true` if open; otherwise `false`.
         */
        bool isOpen() const { return _open; }

        /**
         * @brief Returns send.
         *
         * @param[in] data Input data to read, decode, copy, or process.
         *
         * @return `true` if send; otherwise `false`.
         */
        bool send(uint8_t data);
        /**
         * @brief Returns send.
         *
         * @param[in] data Input data to read, decode, copy, or process.
         * @param[in] length Number of bytes or elements covered by the operation.
         *
         * @return `true` if send; otherwise `false`.
         */
        bool send(const uint8_t *data, size_t length);

        /**
         * @brief Updates the Port for the current service cycle.
         */
        void update();

        /**
         * @brief Notifies about error.
         */
        void notifyError();
        /**
         * @brief Receives data/messages from this transport.
         *
         * @param[in] message Message to process.
         */
        void receive(const std::vector<uint8_t> &message);

    private:
        /**
         * @brief Opens the requested page, file, dialog, or resource.
         */
        void open();
        /**
         * @brief Closes the active page, file, dialog, or resource.
         */
        void close();

        std::string _portIn; ///< Configured host MIDI input port name.
        std::string _portOut; ///< Configured host MIDI output port name.
        RecvHandler _recvHandler; ///< Callback invoked for recv activity.
        ConnectHandler _connectHandler; ///< Callback invoked when a connection is established.
        DisconnectHandler _disconnectHandler; ///< Callback invoked when a connection is closed.

        /**
         * @brief Whether open is true in the current state.
         */
        bool _open = false; ///< True while the corresponding MIDI input/output port is open.
        RtMidiIn _input; ///< RtMidi input endpoint used by this simulator MIDI port.
        RtMidiOut _output; ///< RtMidi output endpoint used by this simulator MIDI port.

        /**
         * @brief Whether first open attempt is true in the current state.
         */
        bool _firstOpenAttempt = true; ///< True until the simulator has made its first attempt to open this MIDI port.
        /**
         * @brief Whether error is true in the current state.
         */
        bool _error = false; ///< True after opening or operating the MIDI port has reported an error.

        std::deque<std::vector<uint8_t>> _recvQueue; ///< Raw host MIDI packets waiting to be consumed by the simulator frontend.
        std::mutex _recvQueueMutex; ///< Mutex protecting `_recvQueue` across the MIDI callback and simulator thread.
    };

    /**
     * @brief Constructs a Midi instance.
     */
    Midi();

    /**
     * @brief Registers port.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     */
    void registerPort(std::shared_ptr<Port> port);

    /**
     * @brief Updates the Midi for the current service cycle.
     */
    void update();

    /**
     * @brief Writes available MIDI/transport ports to diagnostic output.
     */
    void dumpPorts();

private:
    std::vector<std::shared_ptr<Port>> _ports; ///< Shared host-MIDI port objects currently opened by the frontend.
};

} // namespace sim
