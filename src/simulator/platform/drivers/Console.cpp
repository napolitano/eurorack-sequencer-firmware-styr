/**
 * @file Console.cpp
 * @brief Implements the Console component used by the simulator driver.
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
#include "Console.h"

#include <iostream>

void Console::write(char c) {
    send(c);
}

void Console::write(const char *s) {
    while (*s != '\0') {
        send(*s++);
    }
}

void Console::write(const char *s, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        send(s[i]);
    }
}

void Console::write(const std::string &s) {
    for (const auto c : s) {
        send(char(c));
    }
}

void Console::send(char c) {
    std::cout << c;
}

extern "C" {

int _write(int file, char *data, int len) {
    Console::write(data, len);
    return len;
}

}
