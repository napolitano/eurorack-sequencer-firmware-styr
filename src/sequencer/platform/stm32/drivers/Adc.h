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

/**
 * @brief Provides the adc hardware/platform abstraction.
 */
class Adc {
public:
    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_ADC_CHANNELS; ///< Number of physical ADC input channels configured for this hardware.

    /**
     * @brief Initializes the Adc and its runtime resources.
     */
    void init();

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
     * @brief Driver value representing channels.
     */
    uint16_t _channels[Channels]; ///< Most recent ADC sample for each configured input channel.
};
