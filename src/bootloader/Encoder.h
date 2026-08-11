/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the bootloader.
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

class Encoder {
public:
    static void init();
    static void deinit();

    static void process();

    static bool down() { return _down; }

    static void reset();
    static bool pressed();
    static bool released();
    static int value();

private:
    static bool _down;
    static bool _pressed;
    static bool _released;
    static int _value;
};
