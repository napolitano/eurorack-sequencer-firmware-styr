/**
 * @file Adc.h
 * @brief Declares the Adc component used by the simulator driver.
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

#include <array>

#include <cmath>
#include <cstdint>

/**
 * @brief Provides the adc hardware/platform abstraction.
 */
class Adc : private sim::TargetInputHandler {
public:
    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_ADC_CHANNELS; ///< Number of simulated ADC input channels.

    /**
     * @brief Constructs a Adc instance.
     */
    Adc() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance())
    {
        for (int channel = 0; channel < Channels; ++channel) {
            _channels[channel] = 0x7fff;
        }

        _simulator.registerTargetInputObserver(this);
    }

    /**
     * @brief Destroys the Adc instance.
     */
    ~Adc() {
        _simulator.unregisterTargetInputObserver(this);
    }

    /**
     * @brief Initializes the Adc and its runtime resources.
     */
    void init() {}

    /**
     * @brief Returns channel.
     *
     * @param[in] index Zero-based channel index.
     *
     * @return MIDI channel in the standard 0..15 internal representation.
     */
    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    /**
     * @brief Writes adc.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write adc.
     */
    void writeAdc(int channel, uint16_t value) override {
        _channels[channel] = value;
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    /**
     * @brief Fixed-capacity storage for channels.
     */
    std::array<uint16_t, Channels> _channels; ///< Most recent raw ADC sample for each simulated channel.
};
