/**
 * @file Config.h
 * @brief Declares the Config component used by the bootloader.
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

#define CONFIG_VERSION_MAJOR        0
#define CONFIG_VERSION_MINOR        2

#define CONFIG_CPU_FREQUENCY        168000000
#define CONFIG_TICK_FREQUENCY       1000


#define CONFIG_LCD_WIDTH            256
#define CONFIG_LCD_HEIGHT           64

#define CONFIG_ENABLE_DEBUG         1

#define CONFIG_UPDATE_FILENAME      "UPDATE.DAT"

#define CONFIG_APPLICATION_ADDR     0x08010000
#define CONFIG_APPLICATION_SIZE     0xF0000
#define CONFIG_VERSION_TAG_OFFSET   0x400
#define CONFIG_VERSION_TAG_MAGIC    0xfadebabe
