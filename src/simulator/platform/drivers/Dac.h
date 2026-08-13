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

/**
 * @brief Provides the dac hardware/platform abstraction.
 */
class Dac {
public:
    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_DAC_CHANNELS; ///< Number of simulated DAC output channels.

    typedef uint16_t Value;

    /**
     * @brief Constructs a Dac instance.
     */
    Dac() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {}

    /**
     * @brief Initializes the Dac and its runtime resources.
     */
    void init() {}

    /**
     * @brief Sets the value.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value New value to store or apply.
     */
    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     */
    void write(int channel) {
        _simulator.writeDac(channel, _values[channel]);
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     */
    void write() {
        for (int channel = 0; channel < Channels; ++channel) {
            write(channel);
        }
    }

private:
    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    Value _values[Channels]; ///< Owned fixed-size collection of values.
};
