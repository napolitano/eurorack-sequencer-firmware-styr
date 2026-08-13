/**
 * @file UpdateImagePolicy.h
 * @brief Defines bootloader update-image size invariants shared by production code and tests.
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

#include "Config.h"
#include "VersionTag.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace BootloaderUpdate {

static const std::size_t ChecksumSize = 16; ///< Size of the MD5 checksum appended to an update file, in bytes. ///< Size of the MD5 checksum appended to an update file, in bytes.
static const std::size_t MinimumPayloadSize = CONFIG_VERSION_TAG_OFFSET + sizeof(VersionTag); ///< Smallest payload that can contain the complete version tag at its fixed application offset, in bytes. ///< Smallest payload that can contain the complete version tag at its fixed application offset, in bytes.
static const std::size_t MaximumPayloadSize = CONFIG_APPLICATION_SIZE; ///< Largest update payload that fits in the application flash partition, in bytes. ///< Largest update payload that fits in the application flash partition, in bytes.
static const std::size_t FlashWordSize = sizeof(uint32_t); ///< Programming granularity used when rounding update payload sizes, in bytes. ///< Programming granularity used when rounding update payload sizes, in bytes.

/**
 * @brief Returns the number of flash programming words required for a byte count.
 *
 * @param[in] byteCount Number of bytes that must fit in flash programming words.
 *
 * @return Computed result in the domain described by this function.
 */
inline std::size_t programWordCount(std::size_t byteCount) {
    return byteCount / FlashWordSize + (byteCount % FlashWordSize != 0 ? 1u : 0u);
}

/**
 * @brief Returns the flash-programmed size after rounding a byte count to whole flash words.
 *
 * @param[in] byteCount Number of bytes that must fit in flash programming words.
 *
 * @return Computed result in the domain described by this function.
 */
inline std::size_t programmedSize(std::size_t byteCount) {
    return programWordCount(byteCount) * FlashWordSize;
}

/**
 * Convert a complete UPDATE.DAT size into its firmware payload size.
 *
 * UPDATE.DAT consists of the raw application image followed by a 16-byte MD5
 * digest. Reject malformed files before subtracting the checksum size so an
 * undersized file can never wrap an unsigned size_t. The payload must contain
 * the complete version tag and fit in the application flash partition.
 */
inline bool payloadSize(std::size_t fileSize, std::size_t &payloadSizeOut) {
    if (fileSize < ChecksumSize) {
        return false;
    }

    const std::size_t candidate = fileSize - ChecksumSize;
    if (candidate < MinimumPayloadSize || candidate > MaximumPayloadSize ||
        programmedSize(candidate) > MaximumPayloadSize) {
        return false;
    }

    payloadSizeOut = candidate;
    return true;
}

static_assert(std::is_standard_layout<VersionTag>::value, "VersionTag must remain a standard-layout update ABI type");
static_assert(sizeof(VersionTag) == 32, "VersionTag is part of the bootloader update ABI");
static_assert(offsetof(VersionTag, magic) == 0, "VersionTag.magic ABI offset changed");
static_assert(offsetof(VersionTag, name) == 4, "VersionTag.name ABI offset changed");
static_assert(offsetof(VersionTag, major) == 28, "VersionTag.major ABI offset changed");
static_assert(offsetof(VersionTag, minor) == 29, "VersionTag.minor ABI offset changed");
static_assert(offsetof(VersionTag, revision) == 30, "VersionTag.revision ABI offset changed");
static_assert(MinimumPayloadSize <= MaximumPayloadSize, "VersionTag must fit in application flash");

} // namespace BootloaderUpdate
