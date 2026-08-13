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

/// @brief Major component of the firmware version.
#define CONFIG_VERSION_MAJOR        0
/// @brief Minor component of the firmware version.
#define CONFIG_VERSION_MINOR        2

/// @brief STM32 core clock frequency, in hertz.
#define CONFIG_CPU_FREQUENCY        168000000
/// @brief RTOS/system tick frequency, in hertz.
#define CONFIG_TICK_FREQUENCY       1000


/// @brief LCD width, in pixels.
#define CONFIG_LCD_WIDTH            256
/// @brief LCD height, in pixels.
#define CONFIG_LCD_HEIGHT           64

/// @brief Compile-time switch enabling debug support for this target.
#define CONFIG_ENABLE_DEBUG         1

/// @brief SD-card filename recognized by the bootloader as a firmware update image.
#define CONFIG_UPDATE_FILENAME      "UPDATE.DAT"

/// @brief Start address of the application image in STM32 flash.
#define CONFIG_APPLICATION_ADDR     0x08010000
/// @brief Maximum application flash region size, in bytes.
#define CONFIG_APPLICATION_SIZE     0xF0000
/// @brief Byte offset of the firmware version tag within the application image.
#define CONFIG_VERSION_TAG_OFFSET   0x400
/// @brief Magic value used by the bootloader to validate the application version tag.
#define CONFIG_VERSION_TAG_MAGIC    0xfadebabe
