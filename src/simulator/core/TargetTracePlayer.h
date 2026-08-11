/**
 * @file TargetTracePlayer.h
 * @brief Declares the TargetTracePlayer component used by the simulator core.
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

#include "Target.h"
#include "TargetTrace.h"

#include <vector>
#include <memory>

namespace sim {

class Simulator;

struct TracePlayerBase;

class TargetTracePlayer : public TargetTickHandler {
public:
    TargetTracePlayer(const TargetTrace &targetTrace, TargetInputHandler *targetInputHandler, TargetOutputHandler *targetOutputHandler);
    ~TargetTracePlayer();

    const TargetTrace &targetTrace() const { return _targetTrace; }

protected:
    virtual void setTick(uint32_t tick) override;

    const TargetTrace &_targetTrace;
    TargetInputHandler *_targetInputHandler;
    TargetOutputHandler *_targetOutputHandler;

    std::vector<std::unique_ptr<TracePlayerBase>> _tracePlayers;
};

} // namespace sim
