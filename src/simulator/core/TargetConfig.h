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

static const int LcdWidth = 256; ///< Simulated LCD width in pixels. ///< Simulated LCD width in pixels.
static const int LcdHeight = 64; ///< Simulated LCD height in pixels. ///< Simulated LCD height in pixels.
static const int Rows = 8; ///< Number of rows in the simulated button/LED matrix. ///< Number of rows in the simulated button/LED matrix.
static const int ColsButton = 5; ///< Number of button columns exposed by each simulated matrix row. ///< Number of button columns exposed by each simulated matrix row.
static const int ColsLed = 4; ///< Number of LED columns exposed by each simulated matrix row. ///< Number of LED columns exposed by each simulated matrix row.
static const int AdcChannels = 4; ///< Number of simulated ADC input channels. ///< Number of simulated ADC input channels.
static const int DacChannels = 8; ///< Number of simulated DAC output channels. ///< Number of simulated DAC output channels.
static const int GateChannels = 8; ///< Number of simulated gate output channels. ///< Number of simulated gate output channels.
static const int DigitalInputs = 2; ///< Number of simulated digital input channels. ///< Number of simulated digital input channels.
static const int DigitalOutputs = 2; ///< Number of simulated digital output channels. ///< Number of simulated digital output channels.

}
