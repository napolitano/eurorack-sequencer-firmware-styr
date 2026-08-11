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

template<uint8_t Count>
class Debouncer {
public:
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
    bool _state = false;
    uint8_t _counter = 0;
};
