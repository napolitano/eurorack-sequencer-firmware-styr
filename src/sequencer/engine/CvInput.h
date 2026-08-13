/**
 * @file CvInput.h
 * @brief Declares the CvInput component used by the sequencer engine.
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

#include "Config.h"

#include "drivers/Adc.h"

#include <array>

/**
 * @brief Samples and converts calibrated CV input channels for the sequencer engine.
 */
class CvInput {
public:
    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_CV_INPUT_CHANNELS; ///< Number of sequencer CV input channels.

    /**
     * @brief Constructs a CvInput instance.
     *
     * @param[in] adc ADC driver used to sample control-voltage inputs.
     */
    CvInput(Adc &adc);

    /**
     * @brief Initializes the CvInput and its runtime resources.
     */
    void init();

    /**
     * @brief Updates the CvInput for the current service cycle.
     */
    void update();

    /**
     * @brief Returns channel.
     *
     * @param[in] index Zero-based channel index.
     *
     * @return MIDI channel in the standard 0..15 internal representation.
     */
    float channel(int index) const {
        return _channels[index];
    }

private:
    /**
     * @brief Reference to adc owned by another component.
     */
    Adc &_adc; ///< Reference to adc owned by another component.

    /**
     * @brief Fixed-capacity storage for channels.
     */
    std::array<float, Channels> _channels; ///< Most recent converted input voltage for each CV channel.
};
