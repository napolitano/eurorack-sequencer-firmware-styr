/**
 * @file SystemConfig.h
 * @brief Declares the SystemConfig component used by the sequencer application.
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

#include "Platform.h"

// CPU
/// @brief STM32 core clock frequency, in hertz.
#define CONFIG_CPU_FREQUENCY            168000000
/// @brief RTOS/system tick frequency, in hertz.
#define CONFIG_TICK_FREQUENCY           1000

// Interrupt priorities
/// @brief NVIC priority assigned to the high-resolution timer interrupt.
#define CONFIG_HIGHRES_IRQ_PRIORITY     (0<<4)
/// @brief NVIC priority assigned to the sequencer clock-timer interrupt.
#define CONFIG_CLOCKTIMER_IRQ_PRIORITY  (1<<4)
/// @brief NVIC priority assigned to digital-I/O interrupts.
#define CONFIG_DIO_IRQ_PRIORITY         (2<<4)
/// @brief NVIC priority assigned to MIDI interrupts.
#define CONFIG_MIDI_IRQ_PRIORITY        (3<<4)
/// @brief NVIC priority assigned to LCD/display interrupts.
#define CONFIG_LCD_IRQ_PRIORITY         (4<<4)
/// @brief NVIC priority assigned to console/debug interrupts.
#define CONFIG_CONSOLE_IRQ_PRIORITY     (5<<4)

// printf
/// @brief Temporary formatting buffer size, in bytes.
#define CONFIG_PRINTF_BUFFER            16

// Debugging
/// @brief Compile-time switch enabling debug support for this target.
#define CONFIG_ENABLE_DEBUG             1
/// @brief Compile-time switch enabling the runtime profiler.
#define CONFIG_ENABLE_PROFILER          0
/// @brief Compile-time switch enabling per-task profiling.
#define CONFIG_ENABLE_TASK_PROFILER     1

// Sanitization
/// @brief Compile-time switch enabling runtime model/value sanitization.
#define CONFIG_ENABLE_SANITIZE          1

// USB host
/// @brief Compile-time switch enabling the USB host full-speed driver.
#define CONFIG_ENABLE_USBH_DRIVER_FS    1
/// @brief Compile-time switch enabling USB host debug logging.
#define CONFIG_ENABLE_USBH_DEBUG        0

// LCD
/// @brief LCD width, in pixels.
#define CONFIG_LCD_WIDTH                256
/// @brief LCD height, in pixels.
#define CONFIG_LCD_HEIGHT               64

// Simulator display color
/// @brief Identifier for the yellow simulator display color preset.
#define DISPLAY_YELLOW                  0
/// @brief Identifier for the white simulator display color preset.
#define DISPLAY_WHITE                   1
/// @brief Identifier for the cyan simulator display color preset.
#define DISPLAY_CYAN                    2

#ifndef CONFIG_SIMULATOR_DISPLAY_COLOR
/// @brief Display phosphor/color preset used by the desktop simulator.
#define CONFIG_SIMULATOR_DISPLAY_COLOR  DISPLAY_YELLOW
#endif

// Simulator frontpanel asset suffix.
// Examples:
//   ""        -> assets/frontpanel.png
//   "-white"  -> assets/frontpanel-white.png
#ifndef CONFIG_SIMULATOR_FRONTPANEL_SUFFIX
/// @brief Optional filename suffix selecting an alternate simulator front-panel asset.
#define CONFIG_SIMULATOR_FRONTPANEL_SUFFIX ""
#endif

// Shift registers
/// @brief Number of hardware shift registers used by the panel I/O chain.
#define CONFIG_NUM_SR                   3

// Button Led Matrix
/// @brief Number of rows in the button/LED matrix.
#define CONFIG_BLM_ROWS                 8
/// @brief Number of button columns in the panel matrix.
#define CONFIG_BLM_COLS_BUTTON          5
/// @brief Number of LED columns in the panel matrix.
#define CONFIG_BLM_COLS_LED             4

/// @brief Number of physical function keys available on the panel.
#define CONFIG_FUNCTION_KEY_COUNT       5

// ADC
/// @brief Number of ADC channels provided by the hardware target.
#define CONFIG_ADC_CHANNELS             4

// DAC
/// @brief Number of DAC channels provided by the hardware target.
#define CONFIG_DAC_CHANNELS             8

// SdCard
/// @brief Compile-time switch enabling the SD-card detect input.
#define CONFIG_SDCARD_USE_CARD_DETECT   1
