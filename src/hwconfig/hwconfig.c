/**
 * @file hwconfig.c
 * @brief Implements the hwconfig component used by the hardware configuration firmware.
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
#include "HardwareConfig.h"

const struct HardwareConfigData hwconfig __attribute__((section(".hwconfig"))) = {
    .magic = HARDWARE_CONFIG_MAGIC,
    .version = HARDWARE_CONFIG_VERSION,
    .reverseEncoder = REVERSE_ENCODER,
    .invertLeds = INVERT_LEDS,
    .dacType = DAC_TYPE,
};
