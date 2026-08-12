/**
 * @file Format.h
 * @brief Declares the compact formatter used by the fixed-size Styr bootloader.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstddef>

extern "C" {

/**
 * Write a formatted message to the bootloader serial console.
 *
 * The bootloader deliberately implements only the integer/string conversion
 * subset it uses itself. Keeping the formatter local avoids pulling a generic
 * printf implementation into the fixed 32 KiB image.
 */
int printf(const char *format, ...) __attribute__((__format__(__printf__, 1, 2)));

/**
 * Write a formatted message to a bounded character buffer.
 *
 * Supported conversions: %% %s %d %u %x plus the l length modifier and
 * decimal field width with optional zero padding (for example %02x, %08lx).
 */
int snprintf(char *buffer, std::size_t size, const char *format, ...)
    __attribute__((__format__(__printf__, 3, 4)));

} // extern "C"
