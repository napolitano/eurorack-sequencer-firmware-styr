/**
 * @file KeyPressEventTracker.h
 * @brief Declares the KeyPressEventTracker component used by the sequencer UI.
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

#include "Event.h"

#include "os/os.h"

/**
 * @brief Tracks repeated/held key presses and emits count-aware press events.
 */
class KeyPressEventTracker {
public:
    /**
     * @brief Processes the supplied input for this component.
     *
     * @param[in] key Key identifier associated with the input event.
     *
     * @return Result of process().
     */
    KeyPressEvent process(const Key &key) {
        uint32_t currentTicks = os::ticks();
        uint32_t deltaTicks = currentTicks - _lastTicks;

        if (key.code() != _lastCode || deltaTicks > os::time::ms(300)) {
            _count = 1;
        } else {
            ++_count;
        }

        _lastCode = key.code();
        _lastTicks = currentTicks;

        return KeyPressEvent(Event::Type::KeyPress, key, _count);
    }

private:
    /**
     * @brief Most recently observed code.
     */
    uint8_t _lastCode = Key::None; ///< Key code from the previous key event, used to recognize repeat/hold transitions.
    /**
     * @brief Most recently observed ticks.
     */
    uint32_t _lastTicks = 0; ///< System tick of the previous key event, used for repeat/hold timing.
    uint8_t _count = 1; ///< Consecutive press count for the key currently tracked by this helper.
};
