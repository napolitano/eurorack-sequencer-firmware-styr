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

template<typename T>
class Atomic {
public:
    Atomic() = default;
    Atomic(const T &value) : _value(value) {}

    inline T set(const T &value) {
        os::InterruptLock lock;
        T old = _value;
        _value = value;
        return old;
    }
private:
    T _value = T(0);
};
