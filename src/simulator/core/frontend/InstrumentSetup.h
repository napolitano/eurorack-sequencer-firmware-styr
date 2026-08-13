/**
 * @file InstrumentSetup.h
 * @brief Declares the InstrumentSetup component used by the simulator frontend.
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

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include <memory>

namespace sim {

/**
 * @brief Stores and edits instrument setup configuration.
 */
class InstrumentSetup {
public:
    /**
     * @brief Destroys the InstrumentSetup instance.
     */
    virtual ~InstrumentSetup() {}
    
    /**
     * @brief Sets the gate.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] gate `true` to enable/set gate; `false` to disable/clear it.
     */
    virtual void setGate(int channel, bool gate) {
        _instruments[channel]->setGate(gate);
    }

    /**
     * @brief Sets the cv.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] cv Control-voltage value in the engine/model voltage domain.
     */
    virtual void setCv(int channel, float cv) {
        _instruments[channel]->setCv(cv);
    }

protected:
    std::vector<Instrument::Ptr> _instruments; ///< Owned instrument widgets/audio sources configured for the simulator session.
};

/**
 * @brief Stores and edits sampler setup configuration.
 */
class SamplerSetup : public InstrumentSetup {
public:
    /**
     * @brief Constructs a SamplerSetup instance.
     *
     * @param[in] audio Audio source/buffer processed by the operation.
     */
    SamplerSetup(Audio &audio);
};

/**
 * @brief Stores and edits mixed setup configuration.
 */
class MixedSetup : public InstrumentSetup {
public:
    /**
     * @brief Constructs a MixedSetup instance.
     *
     * @param[in] audio Audio source/buffer processed by the operation.
     */
    MixedSetup(Audio &audio);
};

} // namespace sim
