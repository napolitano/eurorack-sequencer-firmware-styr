/**
 * @file Atomic.h
 * @brief Declares the Atomic component used by the STM32 sequencer OS integration.
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
 * @brief Provides interrupt-protected access to a scalar value.
 */
template<typename T>
/**
 * @brief Provides interrupt-safe access to a small value shared across execution contexts.
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
    T _value = T(0); ///< Value protected by the interrupt lock during atomic replacement. ///< Value protected by the interrupt lock during atomic replacement.
};
