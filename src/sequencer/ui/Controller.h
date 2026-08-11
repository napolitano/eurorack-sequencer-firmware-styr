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

struct ControllerInfo {
    enum class Type : uint8_t {
        Launchpad,
    };

    uint16_t vendorId;
    uint16_t productId;
    Type type;
};

class ControllerManager;

class Controller {
public:
    Controller(ControllerManager &manager, Model &model, Engine &engine);
    virtual ~Controller();

    virtual void update() = 0;

    virtual void recvMidi(uint8_t cable, const MidiMessage &message) = 0;

protected:
    bool sendMidi(uint8_t cable, const MidiMessage &message);

    ControllerManager &_manager;
    Model &_model;
    Engine &_engine;
};
