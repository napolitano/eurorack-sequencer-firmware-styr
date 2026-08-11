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

class ControllerManager {
public:
    ControllerManager(Model &model, Engine &engine);

    void connect(uint16_t vendorId, uint16_t productId);
    void disconnect();

    bool isConnected() { return _controller != nullptr; }

    void update();

    int fps() const { return 50; }

    bool recvMidi(MidiPort port, uint8_t cable, const MidiMessage &message);

private:
    bool sendMidi(uint8_t cable, const MidiMessage &message);

    Model &_model;
    Engine &_engine;
    MidiPort _port;
    Container<LaunchpadController> _controllerContainer;
    Controller *_controller = nullptr;

    friend class Controller;
};
