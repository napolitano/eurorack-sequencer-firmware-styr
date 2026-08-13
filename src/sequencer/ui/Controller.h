/**
 * @file Controller.h
 * @brief Declares the Controller component used by the sequencer UI.
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

#include "model/Model.h"

#include "engine/Engine.h"

#include "core/midi/MidiMessage.h"

/**
 * @brief Describes metadata associated with the enclosing component.
 */
struct ControllerInfo {
    /**
     * @brief Enumerates the supported type values.
     */
    enum class Type : uint8_t {
        Launchpad, ///< Selects the launchpad type.
    };

    /**
     * @brief USB vendor identifier.
     */
    uint16_t vendorId; ///< USB vendor identifier.
    /**
     * @brief USB product identifier.
     */
    uint16_t productId; ///< USB product identifier.
    Type type; ///< Selected type that determines how `Controller` interprets or renders its data.
};

class ControllerManager;

/**
 * @brief Defines the common interface for external sequencer control surfaces.
 */
class Controller {
public:
    /**
     * @brief Constructs a Controller instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] engine Sequencer engine that owns or drives this component.
     */
    Controller(ControllerManager &manager, Model &model, Engine &engine);
    /**
     * @brief Destroys the Controller instance.
     */
    virtual ~Controller();

    /**
     * @brief Updates the Controller for the current service cycle.
     */
    virtual void update() = 0;

    /**
     * @brief Receives a MIDI message from the platform transport.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    virtual void recvMidi(uint8_t cable, const MidiMessage &message) = 0;

protected:
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
     * @brief Reference to manager owned by another component.
     */
    ControllerManager &_manager; ///< Reference to manager owned by another component.
    /**
     * @brief Persistent sequencer model used by this component.
     */
    Model &_model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
};
