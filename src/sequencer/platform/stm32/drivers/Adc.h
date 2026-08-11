/**
 * @file Adc.h
 * @brief Declares the Adc component used by the STM32 sequencer driver.
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

#include <cstdint>
#include <cstdlib>

class Adc {
public:
    static constexpr int Channels = CONFIG_ADC_CHANNELS;

    void init();

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    uint16_t _channels[Channels];
};
