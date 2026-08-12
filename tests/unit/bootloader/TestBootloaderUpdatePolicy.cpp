/**
 * @file TestBootloaderUpdatePolicy.cpp
 * @brief Tests UPDATE.DAT size and application-partition invariants.
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
#include "UpdateImagePolicy.h"

#include <cstddef>
#include <iostream>

namespace {
int failures = 0;

void expect(bool condition, const char *name) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << name << '\n';
    }
}
} // namespace

int main() {
    std::size_t payload = 0x55aa55aa;

    expect(!BootloaderUpdate::payloadSize(0, payload), "empty file rejected");
    expect(payload == 0x55aa55aa, "failure leaves output unchanged");
    expect(!BootloaderUpdate::payloadSize(BootloaderUpdate::ChecksumSize - 1, payload),
           "file shorter than checksum rejected");
    expect(!BootloaderUpdate::payloadSize(BootloaderUpdate::ChecksumSize, payload),
           "checksum-only file rejected");
    expect(!BootloaderUpdate::payloadSize(
               BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize - 1,
               payload),
           "payload shorter than version-tag extent rejected");

    expect(BootloaderUpdate::payloadSize(
               BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize,
               payload),
           "minimum valid payload accepted");
    expect(payload == BootloaderUpdate::MinimumPayloadSize,
           "minimum payload size derived exactly");

    expect(BootloaderUpdate::payloadSize(
               BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize,
               payload),
           "maximum application payload accepted");
    expect(payload == BootloaderUpdate::MaximumPayloadSize,
           "maximum payload size derived exactly");

    payload = 0x12345678;
    expect(!BootloaderUpdate::payloadSize(
               BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize + 1,
               payload),
           "payload exceeding application partition rejected");
    expect(payload == 0x12345678, "oversize failure leaves output unchanged");


    VersionTag validTag = {};
    validTag.magic = CONFIG_VERSION_TAG_MAGIC;
    expect(validTag.isValid(), "configured version-tag magic is accepted");
    validTag.magic ^= 1u;
    expect(!validTag.isValid(), "wrong version-tag magic is rejected");

    expect(BootloaderUpdate::ChecksumSize == 16, "MD5 trailer remains 16 bytes");
    expect(BootloaderUpdate::MaximumPayloadSize == CONFIG_APPLICATION_SIZE,
           "policy max equals application flash partition");
    expect(BootloaderUpdate::MinimumPayloadSize == CONFIG_VERSION_TAG_OFFSET + sizeof(VersionTag),
           "minimum payload includes complete version tag");

    if (failures != 0) {
        std::cerr << failures << " bootloader update-policy test(s) failed\n";
        return 1;
    }

    std::cout << "Bootloader update policy: all cases passed\n";
    return 0;
}
