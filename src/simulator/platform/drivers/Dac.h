/**
 * @file Dac.h
 * @brief Declares the Dac component used by the simulator driver.
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

#include "SystemConfig.h"

#include "core/Simulator.h"

#include <cstdint>
#include <cstdlib>

class Dac {
public:
    static constexpr int Channels = CONFIG_DAC_CHANNELS;

    typedef uint16_t Value;

    Dac() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    void write(int channel) {
        _simulator.writeDac(channel, _values[channel]);
    }

    void write() {
        for (int channel = 0; channel < Channels; ++channel) {
            write(channel);
        }
    }

private:
    sim::Simulator &_simulator;
    Value _values[Channels];
};
