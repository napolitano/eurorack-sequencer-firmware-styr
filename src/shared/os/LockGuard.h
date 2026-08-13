/**
 * @file LockGuard.h
 * @brief Declares the LockGuard component used by the shared OS abstraction.
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

namespace os {

/**
 * @brief Acquires a lock on construction and releases it automatically on scope exit.
 */
class LockGuard {
public:
    /**
     * @brief Constructs a LockGuard instance.
     *
     * @param[in] mutex Mutex acquired on construction and released on destruction.
     */
    LockGuard(os::Mutex &mutex) :
        /**
         * @brief Returns the mutex.
         */
        _mutex(mutex)
    {
        _mutex.take();
    }

    /**
     * @brief Destroys the LockGuard instance.
     */
    ~LockGuard() {
        _mutex.give();
    }

private:
    /**
     * @brief Reference to mutex owned by another component.
     */
    Mutex &_mutex; ///< Reference to mutex owned by another component.
};

} // namespace os
