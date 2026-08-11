/**
 * @file TargetConfig.h
 * @brief Declares the TargetConfig component used by the simulator core.
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

namespace TargetConfig {

static const int LcdWidth = 256;
static const int LcdHeight = 64;
static const int Rows = 8;
static const int ColsButton = 5;
static const int ColsLed = 4;
static const int AdcChannels = 4;
static const int DacChannels = 8;
static const int GateChannels = 8;
static const int DigitalInputs = 2;
static const int DigitalOutputs = 2;

}
