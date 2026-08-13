/**
 * @file EnumUtils.h
 * @brief Declares the EnumUtils component used by the shared core support.
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

/// @brief Defines bitwise operators for an enum class used as a flag set.
#define ENUM_CLASS_OPERATORS(e_) \
    inline e_ operator&(e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a) & static_cast<int>(b)); } \
    inline e_ operator|(e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a) | static_cast<int>(b)); } \
    inline e_ &operator|=(e_& a, e_ b) { a = a | b; return a; }; \
    inline e_ &operator&=(e_& a, e_ b) { a = a & b; return a; }; \
    inline e_ operator~(e_ a) { return static_cast<e_>(~static_cast<int>(a)); }
