/**
 * @file ControllerManager.h
 * @brief Declares the ControllerManager component used by the sequencer UI.
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

#include "Controller.h"
#include "controllers/launchpad/LaunchpadController.h"

#include "model/Model.h"

#include "engine/Engine.h"
#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"
#include "core/utils/Container.h"

/**
 * @brief Detects, owns and routes events to the active external controller.
 */
class ControllerManager {
public:
    /**
     * @brief Constructs a ControllerManager instance.
     *
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] engine Sequencer engine that owns or drives this component.
     */
    ControllerManager(Model &model, Engine &engine);

    /**
     * @brief Establishes the requested connection.
     *
     * @param[in] vendorId USB vendor identifier (VID).
     * @param[in] productId USB product identifier (PID).
     */
    void connect(uint16_t vendorId, uint16_t productId);
    /**
     * @brief Closes the active connection.
     */
    void disconnect();

    /**
     * @brief Reports whether connected.
     *
     * @return `true` if connected; otherwise `false`.
     */
    bool isConnected() { return _controller != nullptr; }

    /**
     * @brief Updates the ControllerManager for the current service cycle.
     */
    void update();

    /**
     * @brief Returns the fps.
     *
     * @return Requested UI refresh rate in frames per second.
     */
    int fps() const { return 50; }

    /**
     * @brief Receives a MIDI message from the platform transport.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if recv midi; otherwise `false`.
     */
    bool recvMidi(MidiPort port, uint8_t cable, const MidiMessage &message);

private:
    /**
     * @brief Sends a MIDI message through the requested output port.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if send midi; otherwise `false`.
     */
    bool sendMidi(uint8_t cable, const MidiMessage &message);

    /**
     * @brief Persistent sequencer model used by this component.
     */
    Model &_model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
    MidiPort _port; ///< MIDI port associated with this state.
    Container<LaunchpadController> _controllerContainer; ///< In-place variant storage that owns the active controller object without heap allocation.
    /**
     * @brief Pointer to controller; `nullptr` denotes that no object/resource is assigned.
     */
    Controller *_controller = nullptr; ///< Pointer to controller; `nullptr` denotes that no object/resource is assigned.

    friend class Controller;
};
