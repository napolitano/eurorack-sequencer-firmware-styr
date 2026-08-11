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

class Adc : private sim::TargetInputHandler {
public:
    static constexpr int Channels = CONFIG_ADC_CHANNELS;

    Adc() {
        for (int channel = 0; channel < Channels; ++channel) {
            _channels[channel] = 0x7fff;
        }

        sim::Simulator::instance().registerTargetInputObserver(this);
    }

    void init() {}

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    void writeAdc(int channel, uint16_t value) override {
        _channels[channel] = value;
    }

    std::array<uint16_t, Channels> _channels;
};
