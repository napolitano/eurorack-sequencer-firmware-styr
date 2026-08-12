/**
 * @file TestBootloaderUpdateFile.cpp
 * @brief Host integration tests for UPDATE.DAT parsing through a fake FatFs backend.
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
#include "UpdateFile.h"
#include "UpdateImagePolicy.h"
#include "FileSystem.h"
#include "Console.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
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
int failures = 0;

void resetFakeFs() {
    fileData.clear();
    filePosition = 0;
    mountResult = openResult = statResult = seekResult = readResult = FR_OK;
    failSeekCall = 0;
    seekCalls = 0;
    failReadCall = 0;
    shortReadCall = 0;
    readCalls = 0;
}

void expect(bool condition, const char *name) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << name << '\n';
    }
}

void expectString(const char *actual, const char *expected, const char *name) {
    expect(std::strcmp(actual, expected) == 0, name);
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

    const std::size_t available = filePosition < fileData.size() ? fileData.size() - filePosition : 0;
    std::size_t count = std::min<std::size_t>(requested, available);
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

int main() {
    char error[64] = {};
    VersionTag version = {};
    std::size_t payloadSize = 0;
    uint8_t md5[16] = {};

    resetFakeFs();
    makeValidImage(4096);
    expect(UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "valid update file opens");
    expect(payloadSize == 4096, "valid payload size excludes MD5 trailer");
    expect(version.isValid(), "valid update version tag is returned");
    expect(version.major == 1 && version.minor == 2 && version.revision == 345,
           "version fields survive update-file parsing");
    expect(filePosition == 0, "successful open rewinds payload to byte zero");
    for (std::size_t i = 0; i < sizeof(md5); ++i) {
        expect(md5[i] == static_cast<uint8_t>(i * 7u + 3u), "checksum trailer is read exactly");
    }

    uint8_t prefix[8] = {};
    expect(UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)), "payload read succeeds after rewind");
    for (std::size_t i = 0; i < sizeof(prefix); ++i) {
        expect(prefix[i] == fileData[i], "payload read starts at byte zero");
    }

    resetFakeFs();
    fileData.assign(15, 0);
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "file shorter than MD5 trailer is rejected without unsigned underflow");
    expectString(error, "invalid update size", "undersized file reports size error");

    resetFakeFs();
    fileData.assign(BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize - 1, 0);
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "payload too short for version tag is rejected");

    resetFakeFs();
    fileData.assign(BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize + 1, 0);
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "payload larger than application flash is rejected");

    resetFakeFs();
    makeValidImage(4096);
    fileData[CONFIG_VERSION_TAG_OFFSET] ^= 0x01;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "invalid version-tag magic is rejected");
    expectString(error, "invalid version tag", "invalid version tag reports explicit error");

    resetFakeFs();
    makeValidImage(4096);
    std::memset(&fileData[CONFIG_VERSION_TAG_OFFSET + 4], 'X', 24);
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "unterminated version-tag name is rejected before string formatting");
    expectString(error, "invalid version tag", "unterminated version tag reports explicit error");

    resetFakeFs();
    makeValidImage(4096);
    shortReadCall = 1;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "short version-tag read is rejected");

    resetFakeFs();
    makeValidImage(4096);
    shortReadCall = 2;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)),
           "short checksum read is rejected");

    resetFakeFs();
    makeValidImage(4096);
    mountResult = FR_DISK_ERR;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "mount failure is propagated");
    expectString(error, "failed to mount (error: 1)", "mount failure reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    openResult = FR_NO_FILE;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "open failure is propagated");


    resetFakeFs();
    makeValidImage(4096);
    statResult = FR_DISK_ERR;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "stat failure is propagated");
    expectString(error, "failed to stat file (error: 1)", "stat failure reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    failSeekCall = 1;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "version seek failure is propagated");
    expectString(error, "failed to seek version (result: 1)", "version seek failure reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 1;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "version-tag read error is propagated");
    expectString(error, "failed to read version tag (error: 1)", "version-tag read error reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    failSeekCall = 2;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "checksum seek failure is propagated");
    expectString(error, "failed to seek checksum (result: 1)", "checksum seek failure reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 2;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "checksum read error is propagated");
    expectString(error, "failed to read checksum (error: 1)", "checksum read error reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    failSeekCall = 3;
    expect(!UpdateFile::open(version, payloadSize, md5, error, sizeof(error)), "final rewind failure rejects update");
    expectString(error, "failed to seek start (result: 1)", "final rewind failure reports FatFs result");

    resetFakeFs();
    makeValidImage(4096);
    filePosition = fileData.size();
    expect(!UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)), "payload EOF is rejected as a zero-byte read");
    expectString(error, "failed to read data (error: 0)", "payload EOF reports read failure");

    resetFakeFs();
    makeValidImage(4096);
    failReadCall = 1;
    expect(!UpdateFile::read(prefix, sizeof(prefix), error, sizeof(error)), "payload FatFs read error is propagated");
    expectString(error, "failed to read data (error: 1)", "payload read error reports FatFs result");

    if (failures != 0) {
        std::cerr << failures << " bootloader UpdateFile test(s) failed\n";
        return 1;
    }
    std::cout << "Bootloader UpdateFile: all cases passed\n";
    return 0;
}
