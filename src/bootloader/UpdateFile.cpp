/**
 * @file UpdateFile.cpp
 * @brief Implements the UpdateFile component used by the bootloader.
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
#include "UpdateFile.h"
#include "FileSystem.h"
#include "Format.h"
#include "UpdateImagePolicy.h"

static FATFS fs;
static FIL fil;

bool UpdateFile::open(VersionTag &version, size_t &size, uint8_t md5[16], char *errorStr, size_t errorLen) {
    FRESULT result;

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to mount (error: %d)", result);
        return false;
    }

    result = f_open(&fil, CONFIG_UPDATE_FILENAME, FA_READ);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to open file (error: %d)", result);
        return false;
    }

    FILINFO info;
    result = f_stat(CONFIG_UPDATE_FILENAME, &info);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to stat file (error: %d)", result);
        return false;
    }

    if (!BootloaderUpdate::payloadSize(static_cast<size_t>(info.fsize), size)) {
        bootSnprintf(errorStr, errorLen, "invalid update size");
        return false;
    }

    result = f_lseek(&fil, CONFIG_VERSION_TAG_OFFSET);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to seek version (result: %d)", result);
        return false;
    }

    UINT bytesRead;
    result = f_read(&fil, &version, static_cast<UINT>(sizeof(version)), &bytesRead);
    if (result != FR_OK || bytesRead != sizeof(VersionTag)) {
        bootSnprintf(errorStr, errorLen, "failed to read version tag (error: %d)", result);
        return false;
    }
    if (!version.isValid() || version.name[sizeof(version.name) - 1] != '\0') {
        bootSnprintf(errorStr, errorLen, "invalid version tag");
        return false;
    }

    result = f_lseek(&fil, size);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to seek checksum (result: %d)", result);
        return false;
    }

    result = f_read(&fil, md5, 16, &bytesRead);
    if (result != FR_OK || bytesRead != 16) {
        bootSnprintf(errorStr, errorLen, "failed to read checksum (error: %d)", result);
        return false;
    }

    return rewind(errorStr, errorLen);
}

bool UpdateFile::rewind(char *errorStr, size_t errorLen) {
    FRESULT result = f_lseek(&fil, 0);
    if (result != FR_OK) {
        bootSnprintf(errorStr, errorLen, "failed to seek start (result: %d)", result);
        return false;
    }

    return true;
}

bool UpdateFile::read(void *readBuf, size_t readLen, char *errorStr, size_t errorLen) {
    uint8_t *readPos = reinterpret_cast<uint8_t *>(readBuf);
    while (readLen > 0) {
        UINT bytesRead;
        FRESULT result = f_read(&fil, readPos, static_cast<UINT>(readLen), &bytesRead);
        if (result != FR_OK || bytesRead == 0) {
            bootSnprintf(errorStr, errorLen, "failed to read data (error: %d)", result);
            return false;
        }
        readPos += bytesRead;
        readLen -= bytesRead;
    }

    return true;
}
