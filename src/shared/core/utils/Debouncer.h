/**
 * @file Debouncer.h
 * @brief Declares the Debouncer component used by the shared core support.
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

#include <cstdint>

/**
 * @brief Filters a sampled boolean signal and exposes its stable debounced state.
 */
template<uint8_t Count>
/**
 * @brief Filters a boolean input until it remains stable for the configured debounce interval.
 */
class Debouncer {
public:
    /**
     * @brief Returns debounce.
     *
     * @param[in] state New state or logical level to apply.
     *
     * @return `true` if debounce; otherwise `false`.
     */
    bool debounce(bool state) {
        if (state != _state) {
            if (++_counter >= Count) {
                _state = state;
                _counter = 0;
            }
        } else {
            _counter = 0;
        }

        return _state;
    }

private:
    /**
     * @brief Whether state is true in the current state.
     */
    bool _state = false; ///< Current stable logical state retained by the debouncer or digital-I/O wrapper.
    /**
     * @brief Runtime value representing counter.
     */
    uint8_t _counter = 0; ///< Counter used to accumulate consecutive samples/events for the state transition.
};
