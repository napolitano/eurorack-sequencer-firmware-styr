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

class LockGuard {
public:
    LockGuard(os::Mutex &mutex) :
        _mutex(mutex)
    {
        _mutex.take();
    }

    ~LockGuard() {
        _mutex.give();
    }

private:
    Mutex &_mutex;
};

} // namespace os
