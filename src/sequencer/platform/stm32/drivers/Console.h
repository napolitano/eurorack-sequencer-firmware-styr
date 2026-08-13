/**
 * @file Console.h
 * @brief Declares the Console component used by the STM32 sequencer driver.
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

#include <string>

/**
 * @brief Provides the console hardware/platform abstraction.
 */
class Console {
public:
    /**
     * @brief Initializes the Console and its runtime resources.
     */
    static void init();

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    static void write(char c);
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] s String/scalar operand supplied by the caller.
     */
    static void write(const char *s);
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] s String/scalar operand supplied by the caller.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    static void write(const char *s, size_t length);
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] s String/scalar operand supplied by the caller.
     */
    static void write(const std::string &s);

private:
    /**
     * @brief Sends the supplied data through this transport/interface.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    static void send(char c);
};
