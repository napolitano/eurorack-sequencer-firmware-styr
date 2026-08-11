/**
 * @file FrameBuffer.h
 * @brief Declares the FrameBuffer component used by the simulator core.
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

#include "TargetConfig.h"

#include <array>

#include <cstdint>

namespace sim {

typedef std::array<uint8_t, TargetConfig::LcdWidth * TargetConfig::LcdHeight> FrameBuffer;

} // namespace sim
