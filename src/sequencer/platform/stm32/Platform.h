/**
 * @file Platform.h
 * @brief Declares the Platform component used by the STM32 sequencer platform.
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

/// @brief Storage attribute placing eligible uninitialized data in STM32 core-coupled RAM; empty on the simulator.
#define CCMRAM_BSS __attribute__((section(".ccmram_bss")))
