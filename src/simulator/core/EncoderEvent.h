/**
 * @file EncoderEvent.h
 * @brief Declares the EncoderEvent component used by the simulator core.
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

namespace sim {

/**
 * @brief Enumerates the supported encoder event values.
 */
enum class EncoderEvent {
    Left, ///< Selects the left encoder event.
    Right, ///< Selects the right encoder event.
    Down, ///< Selects the down encoder event.
    Up, ///< Selects the up encoder event.
};

} // namespace sim
