/**
 * @file InterruptLock.h
 * @brief Declares the InterruptLock component used by the bootloader.
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

#include <libopencm3/cm3/cortex.h>

/**
 * @brief Temporarily masks interrupts for a scoped bootloader critical section.
 */
class InterruptLock {
public:
    /**
     * @brief Constructs a InterruptLock instance.
     */
    InterruptLock() { cm_disable_interrupts(); }
    /**
     * @brief Destroys the InterruptLock instance.
     */
    ~InterruptLock() { cm_enable_interrupts(); }
};
