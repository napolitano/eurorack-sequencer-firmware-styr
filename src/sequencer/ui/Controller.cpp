/**
 * @file Controller.cpp
 * @brief Implements the Controller component used by the sequencer UI.
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
#include "Controller.h"

#include "ui/ControllerManager.h"

Controller::Controller(ControllerManager &manager, Model &model, Engine &engine) :
    _manager(manager),
    _model(model),
    _engine(engine)
{}

Controller::~Controller() {
}

bool Controller::sendMidi(uint8_t cable, const MidiMessage &message) {
    return _manager.sendMidi(cable, message);
}
