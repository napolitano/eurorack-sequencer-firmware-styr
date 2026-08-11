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

class CvInput {
public:
    static constexpr int Channels = CONFIG_CV_INPUT_CHANNELS;

    CvInput(Adc &adc);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

private:
    Adc &_adc;

    std::array<float, Channels> _channels;
};
