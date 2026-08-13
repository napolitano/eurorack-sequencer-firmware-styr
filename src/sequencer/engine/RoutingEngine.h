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

/**
 * @brief Implements routing engine runtime processing.
 */
class RoutingEngine {
public:
    /**
     * @brief Constructs a RoutingEngine instance.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     */
    RoutingEngine(Engine &engine, Model &model);

    /**
     * @brief Updates the RoutingEngine for the current service cycle.
     */
    void update();

    /**
     * @brief Processes MIDI input received by the engine.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if receive midi; otherwise `false`.
     */
    bool receiveMidi(MidiPort port, const MidiMessage &message);

private:
    /**
     * @brief Refreshes routing source values.
     */
    void updateSources();
    /**
     * @brief Applies routing values to destination sinks.
     */
    void updateSinks();

    /**
     * @brief Writes engine target.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] normalized Normalized value in the range expected by the operation.
     */
    void writeEngineTarget(Routing::Target target, float normalized);

    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
    /**
     * @brief Reference to routing owned by another component.
     */
    Routing &_routing; ///< Reference to routing owned by another component.

    /**
     * @brief Last normalized value observed for each routing source.
     *
     * @note MIDI routes keep their value between events, so zero
     *       initialization is required before the first matching message.
     */
    std::array<float, CONFIG_ROUTE_COUNT> _sourceValues {};

    /**
     * @brief Stores route state used while the component is running.
     */
    struct RouteState {
        Routing::Target target = Routing::Target::None; ///< Routing target whose last resolved value is stored in this state entry.
        /**
         * @brief Runtime value representing tracks.
         */
        uint8_t tracks = 0; ///< Track collection owned/stored by this model.
    };

    /**
     * @brief Fixed-capacity storage for route states.
     */
    std::array<RouteState, CONFIG_ROUTE_COUNT> _routeStates; ///< Per-route runtime state used to detect routed-value changes and edge-triggered actions.

    /**
     * @brief Most recently observed play toggle active.
     */
    uint8_t _lastPlayToggleActive = false; ///< Previous routed play-toggle level, retained for rising-edge detection.
    /**
     * @brief Most recently observed record toggle active.
     */
    uint8_t _lastRecordToggleActive = false; ///< Previous routed record-toggle level, retained for rising-edge detection.
};
