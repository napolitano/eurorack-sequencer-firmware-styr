/**
 * @file Format.cpp
 * @brief Implements the compact formatter used by the fixed-size Styr bootloader.
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
#include "Format.h"

#include "Console.h"

#include <cstdarg>
#include <cstdint>

namespace {

struct Output {
    char *buffer;
    std::size_t capacity;
    std::size_t written;
    bool console;

    void put(char value) {
        if (console) {
            Console::write(value);
        } else if (capacity && written + 1 < capacity) {
            buffer[written] = value;
        }
        ++written;
    }

    void finish() {
        if (!console && capacity) {
            buffer[written < capacity ? written : capacity - 1] = '\0';
        }
    }
};

static void writeUnsigned(Output &out, uint32_t value, unsigned base, unsigned width, char pad) {
    static const char digits[] = "0123456789abcdef";
    char reversed[10];
    unsigned used = 0;

    do {
        reversed[used++] = digits[value % base];
        value /= base;
    } while (value);

    while (used < width) {
        out.put(pad);
        --width;
    }
    while (used) {
        out.put(reversed[--used]);
    }
}

static void writeSigned(Output &out, int32_t value, unsigned width, char pad) {
    uint32_t magnitude;
    if (value < 0) {
        out.put('-');
        magnitude = static_cast<uint32_t>(-(value + 1)) + 1u;
        if (width) {
            --width;
        }
    } else {
        magnitude = static_cast<uint32_t>(value);
    }
    writeUnsigned(out, magnitude, 10, width, pad);
}

static int format(Output &out, const char *fmt, va_list args) {
    while (*fmt) {
        if (*fmt != '%') {
            out.put(*fmt++);
            continue;
        }

        ++fmt;
        if (*fmt == '\0') {
            // Preserve a trailing '%' literally and terminate. The previous
            // implementation stepped back to the '%' and could loop forever.
            out.put('%');
            break;
        }
        if (*fmt == '%') {
            out.put('%');
            ++fmt;
            continue;
        }

        char pad = ' ';
        if (*fmt == '0') {
            pad = '0';
            ++fmt;
        }

        unsigned width = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10u + static_cast<unsigned>(*fmt++ - '0');
        }

        bool isLong = false;
        if (*fmt == 'l') {
            isLong = true;
            ++fmt;
        }

        const char conversion = *fmt ? *fmt++ : '\0';
        switch (conversion) {
        case 's': {
            const char *value = va_arg(args, const char *);
            if (!value) {
                value = "(null)";
            }
            while (*value) {
                out.put(*value++);
            }
            break;
        }
        case 'd':
            writeSigned(out, isLong ? static_cast<int32_t>(va_arg(args, long))
                                    : va_arg(args, int),
                        width, pad);
            break;
        case 'u':
        case 'x':
            writeUnsigned(out,
                          isLong ? static_cast<uint32_t>(va_arg(args, unsigned long))
                                 : va_arg(args, unsigned int),
                          conversion == 'x' ? 16u : 10u, width, pad);
            break;
        default:
            out.put('%');
            out.put(conversion);
            break;
        }
    }

    out.finish();
    return static_cast<int>(out.written);
}

} // namespace

extern "C" {

int bootPrintf(const char *formatString, ...) {
    Output out{nullptr, 0, 0, true};
    va_list args;
    va_start(args, formatString);
    const int result = format(out, formatString, args);
    va_end(args);
    return result;
}

int bootSnprintf(char *buffer, std::size_t size, const char *formatString, ...) {
    Output out{buffer, size, 0, false};
    va_list args;
    va_start(args, formatString);
    const int result = format(out, formatString, args);
    va_end(args);
    return result;
}

} // extern "C"
