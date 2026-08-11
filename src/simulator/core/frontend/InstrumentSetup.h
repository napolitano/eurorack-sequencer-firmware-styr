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

class InstrumentSetup {
public:
    virtual ~InstrumentSetup() {}
    
    virtual void setGate(int channel, bool gate) {
        _instruments[channel]->setGate(gate);
    }

    virtual void setCv(int channel, float cv) {
        _instruments[channel]->setCv(cv);
    }

protected:
    std::vector<Instrument::Ptr> _instruments;
};

class SamplerSetup : public InstrumentSetup {
public:
    SamplerSetup(Audio &audio);
};

class MixedSetup : public InstrumentSetup {
public:
    MixedSetup(Audio &audio);
};

} // namespace sim
