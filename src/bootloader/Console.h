/**
 * @file Console.h
 * @brief Declares the Console component used by the bootloader.
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

#include <cstdlib>

class Console {
public:
    static void init();
    static void deinit();

    static void write(char c);
    static void write(const char *s);
    static void write(const char *s, size_t length);

private:
    static void send(char c);
};

