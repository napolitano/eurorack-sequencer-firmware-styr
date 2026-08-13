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

/**
 * @brief Provides target trace player behavior for the desktop simulator.
 */
class TargetTracePlayer : public TargetTickHandler {
public:
    /**
     * @brief Constructs a TargetTracePlayer instance.
     *
     * @param[in] targetTrace Trace/buffer receiving target-side diagnostic data.
     * @param[in] targetInputHandler Callback invoked for replayed target-input records.
     * @param[in] targetOutputHandler Callback invoked for replayed target-output records.
     */
    TargetTracePlayer(const TargetTrace &targetTrace, TargetInputHandler *targetInputHandler, TargetOutputHandler *targetOutputHandler);
    /**
     * @brief Destroys the TargetTracePlayer instance.
     */
    ~TargetTracePlayer();

    /**
     * @brief Returns the target trace.
     *
     * @return Reference to the target trace.
     */
    const TargetTrace &targetTrace() const { return _targetTrace; }

protected:
    /**
     * @brief Sets the tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     */
    virtual void setTick(uint32_t tick) override;

    /**
     * @brief Reference to target trace owned by another component.
     */
    const TargetTrace &_targetTrace; ///< Reference to target trace owned by another component.
    /**
     * @brief Pointer to target input handler; `nullptr` denotes that no object/resource is assigned.
     */
    TargetInputHandler *_targetInputHandler; ///< Pointer to target input handler; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Pointer to target output handler; `nullptr` denotes that no object/resource is assigned.
     */
    TargetOutputHandler *_targetOutputHandler; ///< Pointer to target output handler; `nullptr` denotes that no object/resource is assigned.

    std::vector<std::unique_ptr<TracePlayerBase>> _tracePlayers; ///< Owned trace-player instances, one for each trace stream being replayed.
};

} // namespace sim
