/**
 * @file CvOutput.h
 * @brief Declares the CvOutput component used by the sequencer engine.
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

#include "model/Calibration.h"

#include "drivers/Dac.h"

#include <array>

/**
 * @brief Converts engine CV values to calibrated DAC output values.
 */
class CvOutput {
public:
    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_CV_OUTPUT_CHANNELS; ///< Number of sequencer CV output channels.

    /**
     * @brief Constructs a CvOutput instance.
     *
     * @param[in] dac DAC driver used to emit control-voltage values.
     * @param[in] calibration Calibration data used to convert between raw DAC/ADC and voltage values.
     */
    CvOutput(Dac &dac, const Calibration &calibration);

    /**
     * @brief Initializes the CvOutput and its runtime resources.
     */
    void init();

    /**
     * @brief Updates the CvOutput for the current service cycle.
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

    /**
     * @brief Sets the channel.
     *
     * @param[in] index Zero-based channel index.
     * @param[in] value New channel to store or apply.
     */
    void setChannel(int index, float value) {
        _channels[index] = value;
    }

private:
    /**
     * @brief Reference to dac owned by another component.
     */
    Dac &_dac; ///< Reference to dac owned by another component.
    /**
     * @brief Reference to calibration owned by another component.
     */
    const Calibration &_calibration; ///< Reference to calibration owned by another component.
    /**
     * @brief Fixed-capacity storage for channels.
     */
    std::array<float, Channels> _channels; ///< Current output voltage for each CV channel, indexed by hardware channel.
};
