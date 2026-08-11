/**
 * @file CvOutput.cpp
 * @brief Implements the CvOutput component used by the sequencer engine.
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
#include "CvOutput.h"

#include "core/math/Math.h"

CvOutput::CvOutput(Dac &dac, const Calibration &calibration) :
    _dac(dac),
    _calibration(calibration)
{}

void CvOutput::init() {
    _channels.fill(0.f);
}

void CvOutput::update() {
    for (int i = 0; i < Channels; ++i) {
        _dac.setValue(i, _calibration.cvOutput(i).voltsToValue(_channels[i]));
    }
    _dac.write();
}
