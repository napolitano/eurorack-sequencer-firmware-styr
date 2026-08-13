/**
 * @file Atomic.h
 * @brief Declares the Atomic component used by the simulator OS integration.
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

#include "os/os.h"

/**
 * @brief Provides atomic behavior for the desktop simulator.
 */
template<typename T>
/**
 * @brief Provides atomic behavior for the desktop simulator.
 */
class Atomic {
public:
    /**
     * @brief Constructs a Atomic instance.
     */
    Atomic() = default;
    /**
     * @brief Constructs a Atomic instance.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     */
    Atomic(const T &value) : _value(value) {}

    /**
     * @brief Returns the protected value while holding the simulator interrupt lock.
     *
     * @return Snapshot of the currently stored value.
     */
    inline T get() const {
        os::InterruptLock lcok;
        return _value;
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] value Replacement value to store atomically.
     *
     * @return Value that was stored before the replacement.
     */
    inline T set(const T &value) {
        os::InterruptLock lock;
        T old = _value;
        _value = value;
        return old;
    }
private:
    /**
     * @brief Returns the t.
     *
     * @return The t value.
     */
    T _value = T(0); ///< Value protected by the simulator interrupt lock. ///< Value protected by the simulator interrupt lock.
};
