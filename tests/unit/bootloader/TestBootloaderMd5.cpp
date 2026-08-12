/**
 * @file TestBootloaderMd5.cpp
 * @brief Verifies the MD5 implementation used to authenticate UPDATE.DAT payload integrity.
 *
 * @author Axel Napolitano — Styr test integration
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt. MD5 implementation retains its upstream public-domain notice.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include "MD5.h"

#include <cstddef>
#include <cstring>
#include <iostream>

namespace {
int failures = 0;

unsigned nibble(char c) {
    if (c >= '0' && c <= '9') return static_cast<unsigned>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<unsigned>(c - 'a' + 10);
    return 0xffu;
}

bool matches(const unsigned char digest[16], const char *hex) {
    for (std::size_t i = 0; i < 16; ++i) {
        const unsigned hi = nibble(hex[i * 2]);
        const unsigned lo = nibble(hex[i * 2 + 1]);
        if (hi > 15 || lo > 15 || digest[i] != static_cast<unsigned char>((hi << 4) | lo)) {
            return false;
        }
    }
    return true;
}

void checkVector(const char *name, const char *message, const char *expected) {
    MD5 md5;
    md5.update(message, static_cast<unsigned long>(std::strlen(message)));
    MD5::Sum sum;
    md5.finish(sum);
    if (!matches(sum, expected)) {
        ++failures;
        std::cerr << "FAILED: MD5 vector " << name << '\n';
    }
}
}

int main() {
    checkVector("empty", "", "d41d8cd98f00b204e9800998ecf8427e");
    checkVector("abc", "abc", "900150983cd24fb0d6963f7d28e17f72");
    checkVector("message digest", "message digest", "f96b697d7cb7938d525a2f31aaf161d0");

    // Verify incremental operation as used by the bootloader's 1 KiB read loop.
    MD5 chunked;
    chunked.update("The quick ", 10);
    chunked.update("brown fox ", 10);
    chunked.update("jumps over the lazy dog", 23);
    MD5::Sum sum;
    chunked.finish(sum);
    if (!matches(sum, "9e107d9d372bb6826bd81d3542a419d6")) {
        ++failures;
        std::cerr << "FAILED: MD5 incremental vector\n";
    }

    if (failures != 0) {
        std::cerr << failures << " bootloader MD5 test(s) failed\n";
        return 1;
    }
    std::cout << "Bootloader MD5: all cases passed\n";
    return 0;
}
