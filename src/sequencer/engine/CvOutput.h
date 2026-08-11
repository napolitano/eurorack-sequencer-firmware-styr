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

class CvOutput {
public:
    static constexpr int Channels = CONFIG_CV_OUTPUT_CHANNELS;

    CvOutput(Dac &dac, const Calibration &calibration);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

    void setChannel(int index, float value) {
        _channels[index] = value;
    }

private:
    Dac &_dac;
    const Calibration &_calibration;
    std::array<float, Channels> _channels;
};
