/**
 * @file CvInput.cpp
 * @brief Implements the CvInput component used by the sequencer engine.
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
#include "CvInput.h"

CvInput::CvInput(Adc &adc) :
    _adc(adc)
{
}

void CvInput::init() {
    _channels.fill(0.f);
}

void CvInput::update() {
    for (int i = 0; i < Channels; ++i) {
        _channels[i] = 5.f - _adc.channel(i) / 6553.5f;
    }
}
