/**
 * @file Config.h
 * @brief Declares the Config component used by the sequencer application.
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

#include "SystemConfig.h"
#include "Colors.h"
#include "Texts.h"

// Version tag
/// @brief Magic value used to identify a valid Styr firmware version tag.
#define CONFIG_VERSION_MAGIC            0xfadebabe
/// @brief Human-readable firmware name embedded in the version tag.
#define CONFIG_VERSION_NAME             "STYR SEQUENCER"
/// @brief Major component of the firmware version.
#define CONFIG_VERSION_MAJOR            0
/// @brief Minor component of the firmware version.
#define CONFIG_VERSION_MINOR            1
/// @brief Revision/patch component of the firmware version.
#define CONFIG_VERSION_REVISION         46
/// @brief Build/release date associated with the configured firmware version.
#define CONFIG_VERSION_DATE             "2026-04-04"

// Task priorities
/// @brief RTOS priority assigned to the hardware-driver task.
#define CONFIG_DRIVER_TASK_PRIORITY     5
/// @brief RTOS priority assigned to the sequencer-engine task.
#define CONFIG_ENGINE_TASK_PRIORITY     4
/// @brief RTOS priority assigned to the USB-host task.
#define CONFIG_USBH_TASK_PRIORITY       3
/// @brief RTOS priority assigned to the UI task.
#define CONFIG_UI_TASK_PRIORITY         2
/// @brief RTOS priority assigned to the file/storage task.
#define CONFIG_FILE_TASK_PRIORITY       1
/// @brief RTOS priority assigned to the profiler task.
#define CONFIG_PROFILER_TASK_PRIORITY   0

// Task stack sizes
/// @brief Stack allocation for the hardware-driver task, in platform stack units.
#define CONFIG_DRIVER_TASK_STACK_SIZE   1024
/// @brief Stack allocation for the sequencer-engine task, in platform stack units.
#define CONFIG_ENGINE_TASK_STACK_SIZE   4096
/// @brief Stack allocation for the USB-host task, in platform stack units.
#define CONFIG_USBH_TASK_STACK_SIZE     2048
/// @brief Stack allocation for the UI task, in platform stack units.
#define CONFIG_UI_TASK_STACK_SIZE       4096
/// @brief Stack allocation for the file/storage task, in platform stack units.
#define CONFIG_FILE_TASK_STACK_SIZE     2048
/// @brief Stack allocation for the profiler task, in platform stack units.
#define CONFIG_PROFILER_TASK_STACK_SIZE 2048

// Settings flash storage
/// @brief STM32 flash sector reserved for persistent settings.
#define CONFIG_SETTINGS_FLASH_SECTOR    3
/// @brief Start address of the persistent settings flash region.
#define CONFIG_SETTINGS_FLASH_ADDR      0x0800C000

// Parts per quarter note
/// @brief Internal sequencer clock resolution, in pulses per quarter note.
#define CONFIG_PPQN                     192

// Sequence parts per quarter note resolution
/// @brief Sequence-step timing resolution, in pulses per quarter note.
#define CONFIG_SEQUENCE_PPQN            48

// Default UI frames per second
/// @brief Default target UI refresh rate, in frames per second.
#define CONFIG_DEFAULT_UI_FPS           50

// CV/Gate channels
/// @brief Number of logical sequencer output channels.
#define CONFIG_CHANNEL_COUNT            8

// CV inputs
/// @brief Number of CV input channels exposed to the sequencer.
#define CONFIG_CV_INPUT_CHANNELS        4

// CV outputs
/// @brief Number of CV output channels exposed to the sequencer.
#define CONFIG_CV_OUTPUT_CHANNELS       8

// Model
/// @brief Number of patterns stored per project.
#define CONFIG_PATTERN_COUNT            16
/// @brief Number of snapshots stored per project.
#define CONFIG_SNAPSHOT_COUNT           1
/// @brief Number of song/arrangement slots available in a project.
#define CONFIG_SONG_SLOT_COUNT          64
/// @brief Number of sequencer tracks in a project.
#define CONFIG_TRACK_COUNT              8
/// @brief Maximum number of steps in a sequence.
#define CONFIG_STEP_COUNT               64
/// @brief Maximum number of modulation/routing entries in a project.
#define CONFIG_ROUTE_COUNT              16
/// @brief Number of configurable MIDI output mappings.
#define CONFIG_MIDI_OUTPUT_COUNT        16
/// @brief Number of user-defined scales stored in a project.
#define CONFIG_USER_SCALE_COUNT         4
/// @brief Maximum number of notes/degrees stored in one user-defined scale.
#define CONFIG_USER_SCALE_SIZE          32

// Features
/// @brief Compile-time switch enabling the Asteroids easter egg.
#define CONFIG_ENABLE_ASTEROIDS
//#define CONFIG_ENABLE_INTRO
/// @brief Compile-time switch enabling Styr startup-page enhancements.
#define CONFIG_ENABLE_STARTPAGE_ENHANCEMENTS
/// @brief Compile-time switch enabling enhanced Note editing behavior.
#define CONFIG_ENABLE_NOTE_EDIT_ENHANCEMENTS
/// @brief Compile-time switch enabling enhanced Curve editing behavior.
#define CONFIG_ENABLE_CURVE_EDIT_ENHANCEMENTS
/// @brief Compile-time switch enabling enhanced Song editing behavior.
#define CONFIG_ENABLE_SONG_EDIT_ENHANCEMENTS
/// @brief Compile-time switch enabling the advanced-settings UI/model.
#define CONFIG_ADVANCED_SETTINGS
/// @brief Compile-time switch enabling the Acid Bassline generator.
#define CONFIG_ACID_BASS_GENERATOR
/// @brief Compile-time switch enabling Styr Overview-page enhancements.
#define CONFIG_OVERVIEW_ENHANCEMENTS
/// @brief Compile-time compatibility fix for inherited scale-definition defects.
#define FIX_BROKEN_SCALES
/// @brief Compile-time compatibility fix for inherited scale-change behavior.
#define FIX_BROKEN_SCALE_CHANGE
/// @brief Compile-time switch enabling configurable encoder direction inversion.
#define CONFIG_ENABLE_ENCODER_INVERT
/// @brief Compile-time switch disabling the hardware watchdog.
#define CONFIG_DISABLE_WATCHDOG
