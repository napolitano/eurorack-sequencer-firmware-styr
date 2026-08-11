/**
 * @file RoutingEngine.h
 * @brief Declares the RoutingEngine component used by the sequencer engine.
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

#include "MidiPort.h"

#include "model/Model.h"

#include "core/midi/MidiMessage.h"

#include <array>

#include <cstdint>

class Engine;

class RoutingEngine {
public:
    RoutingEngine(Engine &engine, Model &model);

    void update();

    bool receiveMidi(MidiPort port, const MidiMessage &message);

private:
    void updateSources();
    void updateSinks();

    void writeEngineTarget(Routing::Target target, float normalized);

    Engine &_engine;
    Routing &_routing;

    /**
     * @brief Last normalized value observed for each routing source.
     *
     * @note MIDI routes keep their value between events, so zero
     *       initialization is required before the first matching message.
     */
    std::array<float, CONFIG_ROUTE_COUNT> _sourceValues {};

    struct RouteState {
        Routing::Target target = Routing::Target::None;
        uint8_t tracks = 0;
    };

    std::array<RouteState, CONFIG_ROUTE_COUNT> _routeStates;

    uint8_t _lastPlayToggleActive = false;
    uint8_t _lastRecordToggleActive = false;
};
