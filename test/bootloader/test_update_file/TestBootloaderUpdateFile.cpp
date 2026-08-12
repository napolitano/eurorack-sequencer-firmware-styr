/**
 * @file TestBootloaderUpdateFile.cpp
 * @brief Unity host integration tests for UPDATE.DAT parsing through a fake FatFs backend.
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
#include <unity.h>

#include "Console.h"
#include "FileSystem.h"
#include "UpdateFile.h"
#include "UpdateImagePolicy.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace {

std::vector<uint8_t> fileData;
std::size_t filePosition = 0;
FRESULT mountResult = FR_OK;
FRESULT openResult = FR_OK;
FRESULT statResult = FR_OK;
FRESULT seekResult = FR_OK;
FRESULT readResult = FR_OK;
int failSeekCall = 0;
int seekCalls = 0;
int failReadCall = 0;
int shortReadCall = 0;
int readCalls = 0;
std::size_t maxReadPerCall = std::numeric_limits<std::size_t>::max();

void resetFakeFs() {
    fileData.clear();
    filePosition = 0;
    mountResult = openResult = statResult = seekResult = readResult = FR_OK;
    failSeekCall = 0;
    seekCalls = 0;
    failReadCall = 0;
    shortReadCall = 0;
    readCalls = 0;
    maxReadPerCall = std::numeric_limits<std::size_t>::max();
}

void makeValidImage(std::size_t payloadSize) {
    fileData.assign(payloadSize + BootloaderUpdate::ChecksumSize, 0);
    VersionTag tag = {};
    tag.magic = CONFIG_VERSION_TAG_MAGIC;
    std::strncpy(tag.name, "styr", sizeof(tag.name) - 1);
    tag.major = 1;
    tag.minor = 2;
    tag.revision = 345;
    std::memcpy(&fileData[CONFIG_VERSION_TAG_OFFSET], &tag, sizeof(tag));
    for (std::size_t i = 0; i < BootloaderUpdate::ChecksumSize; ++i) {
        fileData[payloadSize + i] = static_cast<uint8_t>(i * 7u + 3u);
    }
}

} // namespace

// Format.cpp references the UART Console on its printf path. UpdateFile only
// uses the bounded formatter, but provide the symbol so the production
// translation unit is linked unchanged in this host test.
void Console::write(char) {}

extern "C" {
FRESULT f_mount(FATFS *, const TCHAR *, BYTE) {
    return mountResult;
}

FRESULT f_open(FIL *, const TCHAR *, BYTE) {
    return openResult;
}

FRESULT f_stat(const TCHAR *, FILINFO *info) {
    if (statResult == FR_OK) {
        info->fsize = static_cast<FSIZE_t>(fileData.size());
    }
    return statResult;
}

FRESULT f_lseek(FIL *, FSIZE_t offset) {
    ++seekCalls;
    if (failSeekCall == seekCalls) {
        return FR_DISK_ERR;
    }
    if (seekResult == FR_OK) {
        filePosition = static_cast<std::size_t>(offset);
    }
    return seekResult;
}

FRESULT f_read(FIL *, void *buffer, UINT requested, UINT *bytesRead) {
    ++readCalls;
    if (failReadCall == readCalls) {
        *bytesRead = 0;
        return FR_DISK_ERR;
    }
    if (readResult != FR_OK) {
        *bytesRead = 0;
        return readResult;
    }

    const std::size_t available =
        filePosition < fileData.size() ? fileData.size() - filePosition : 0;
    std::size_t count = std::min<std::size_t>(requested, available);
    count = std::min<std::size_t>(count, maxReadPerCall);
    if (shortReadCall == readCalls && count > 0) {
        --count;
    }
    if (count > 0) {
        std::memcpy(buffer, &fileData[filePosition], count);
    }
    filePosition += count;
    *bytesRead = static_cast<UINT>(count);
    return FR_OK;
}
}

void setUp() {
    resetFakeFs();
}

void tearDown() {}

void test_valid_update_opens_rewinds_and_reads_payload() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(4096);
    TEST_ASSERT_TRUE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_size_t(4096, payloadSize);
    TEST_ASSERT_TRUE(version.isValid());
    TEST_ASSERT_EQUAL_STRING("styr", version.name);
    TEST_ASSERT_EQUAL_UINT8(1, version.major);
    TEST_ASSERT_EQUAL_UINT8(2, version.minor);
    TEST_ASSERT_EQUAL_UINT16(345, version.revision);
    TEST_ASSERT_EQUAL_size_t(0, filePosition);

    for (std::size_t i = 0; i < sizeof(md5); ++i) {
        TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(i * 7u + 3u), md5[i]);
    }

    uint8_t prefix[8] = {};
    TEST_ASSERT_TRUE(UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)));
    TEST_ASSERT_EQUAL_MEMORY(fileData.data(), prefix, sizeof(prefix));
}

void test_minimum_and_maximum_valid_payload_sizes_open_successfully() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(BootloaderUpdate::MinimumPayloadSize);
    TEST_ASSERT_TRUE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_size_t(BootloaderUpdate::MinimumPayloadSize, payloadSize);

    resetFakeFs();
    makeValidImage(BootloaderUpdate::MaximumPayloadSize);
    TEST_ASSERT_TRUE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_size_t(BootloaderUpdate::MaximumPayloadSize, payloadSize);
}

void test_invalid_payload_sizes_are_rejected() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0x11223344u;
    uint8_t md5[16] = {};

    fileData.assign(15, 0);
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("invalid update size", error);
    TEST_ASSERT_EQUAL_size_t(0x11223344u, payloadSize);

    resetFakeFs();
    fileData.assign(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize - 1,
        0);
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("invalid update size", error);

    resetFakeFs();
    fileData.assign(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize + 1,
        0);
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("invalid update size", error);
}

void test_invalid_or_unterminated_version_tag_is_rejected() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(4096);
    fileData[CONFIG_VERSION_TAG_OFFSET] ^= 0x01;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("invalid version tag", error);

    resetFakeFs();
    makeValidImage(4096);
    std::memset(&fileData[CONFIG_VERSION_TAG_OFFSET + 4], 'X', 24);
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("invalid version tag", error);
}

void test_short_version_and_checksum_reads_are_rejected() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(4096);
    shortReadCall = 1;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read version tag (error: 0)", error);

    resetFakeFs();
    makeValidImage(4096);
    shortReadCall = 2;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read checksum (error: 0)", error);
}

void test_mount_open_and_stat_failures_are_propagated() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(4096);
    mountResult = FR_DISK_ERR;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to mount (error: 1)", error);

    resetFakeFs();
    makeValidImage(4096);
    openResult = FR_NO_FILE;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to open file (error: 4)", error);

    resetFakeFs();
    makeValidImage(4096);
    statResult = FR_DISK_ERR;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to stat file (error: 1)", error);
}

void test_seek_and_metadata_read_failures_are_propagated() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    makeValidImage(4096);
    failSeekCall = 1;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to seek version (result: 1)", error);

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 1;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read version tag (error: 1)", error);

    resetFakeFs();
    makeValidImage(4096);
    failSeekCall = 2;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to seek checksum (result: 1)", error);

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 2;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read checksum (error: 1)", error);

    resetFakeFs();
    makeValidImage(4096);
    failSeekCall = 3;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to seek start (result: 1)", error);
}

void test_payload_read_accepts_partial_fatfs_reads_until_request_is_complete() {
    char error[64] = {};
    uint8_t buffer[37] = {};

    makeValidImage(4096);
    maxReadPerCall = 5;
    TEST_ASSERT_TRUE(UpdateFile::read(buffer, sizeof(buffer), error, sizeof(error)));
    TEST_ASSERT_EQUAL_MEMORY(fileData.data(), buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(8, readCalls);
    TEST_ASSERT_EQUAL_size_t(sizeof(buffer), filePosition);
}

void test_zero_length_payload_read_is_a_noop() {
    char error[64] = {};
    uint8_t sentinel = 0xa5;

    makeValidImage(4096);
    TEST_ASSERT_TRUE(UpdateFile::read(&sentinel, 0, error, sizeof(error)));
    TEST_ASSERT_EQUAL_UINT8(0xa5, sentinel);
    TEST_ASSERT_EQUAL_INT(0, readCalls);
    TEST_ASSERT_EQUAL_size_t(0, filePosition);
}

void test_payload_read_eof_and_fatfs_errors_are_rejected() {
    char error[64] = {};
    uint8_t prefix[8] = {};

    makeValidImage(4096);
    filePosition = fileData.size();
    TEST_ASSERT_FALSE(UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read data (error: 0)", error);

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 1;
    TEST_ASSERT_FALSE(UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to read data (error: 1)", error);
}

void test_rewind_success_and_failure_paths_are_explicit() {
    char error[64] = {};

    makeValidImage(4096);
    filePosition = 1234;
    TEST_ASSERT_TRUE(UpdateFile::rewind(error, sizeof(error)));
    TEST_ASSERT_EQUAL_size_t(0, filePosition);

    resetFakeFs();
    makeValidImage(4096);
    seekResult = FR_DISK_ERR;
    TEST_ASSERT_FALSE(UpdateFile::rewind(error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("failed to seek start (result: 1)", error);
}

void test_error_formatting_remains_bounded_for_tiny_error_buffers() {
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};
    char error[5] = {'X', 'X', 'X', 'X', 'X'};

    makeValidImage(4096);
    mountResult = FR_DISK_ERR;
    TEST_ASSERT_FALSE(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)));
    TEST_ASSERT_EQUAL_STRING("fail", error);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_valid_update_opens_rewinds_and_reads_payload);
    RUN_TEST(test_minimum_and_maximum_valid_payload_sizes_open_successfully);
    RUN_TEST(test_invalid_payload_sizes_are_rejected);
    RUN_TEST(test_invalid_or_unterminated_version_tag_is_rejected);
    RUN_TEST(test_short_version_and_checksum_reads_are_rejected);
    RUN_TEST(test_mount_open_and_stat_failures_are_propagated);
    RUN_TEST(test_seek_and_metadata_read_failures_are_propagated);
    RUN_TEST(test_payload_read_accepts_partial_fatfs_reads_until_request_is_complete);
    RUN_TEST(test_zero_length_payload_read_is_a_noop);
    RUN_TEST(test_payload_read_eof_and_fatfs_errors_are_rejected);
    RUN_TEST(test_rewind_success_and_failure_paths_are_explicit);
    RUN_TEST(test_error_formatting_remains_bounded_for_tiny_error_buffers);
    return UNITY_END();
}
