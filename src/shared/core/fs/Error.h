/**
 * @file Error.h
 * @brief Declares the Error component used by the shared filesystem support.
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

#include "ff/ff.h"

namespace fs {

/**
 * @brief Enumerates the supported error values.
 */
enum Error {
    OK = FR_OK, ///< Selects the ok error.
    DISK_ERR = FR_DISK_ERR, ///< Selects the disk err error.
    INT_ERR = FR_INT_ERR, ///< Selects the int err error.
    NOT_READY = FR_NOT_READY, ///< Selects the not ready error.
    NO_FILE = FR_NO_FILE, ///< Selects the no file error.
    NO_PATH = FR_NO_PATH, ///< Selects the no path error.
    INVALID_NAME = FR_INVALID_NAME, ///< Selects the invalid name error.
    DENIED = FR_DENIED, ///< Selects the denied error.
    EXIST = FR_EXIST, ///< Selects the exist error.
    INVALID_OBJECT = FR_INVALID_OBJECT, ///< Selects the invalid object error.
    WRITE_PROTECTED = FR_WRITE_PROTECTED, ///< Selects the write protected error.
    INVALID_DRIVE = FR_INVALID_DRIVE, ///< Selects the invalid drive error.
    NOT_ENABLED = FR_NOT_ENABLED, ///< Selects the not enabled error.
    NO_FILESYSTEM = FR_NO_FILESYSTEM, ///< Selects the no filesystem error.
    MKFS_ABORTED = FR_MKFS_ABORTED, ///< Selects the mkfs aborted error.
    TIMEOUT = FR_TIMEOUT, ///< Selects the timeout error.
    LOCKED = FR_LOCKED, ///< Selects the locked error.
    NOT_ENOUGH_CORE = FR_NOT_ENOUGH_CORE, ///< Selects the not enough core error.
    TOO_MANY_OPEN_FILES = FR_TOO_MANY_OPEN_FILES, ///< Selects the too many open files error.
    INVALID_PARAMETER = FR_INVALID_PARAMETER, ///< Selects the invalid parameter error.
    DISK_FULL, ///< Selects the disk full error.
    END_OF_FILE, ///< Selects the end of file error.
    INVALID_CHECKSUM, ///< Selects the invalid checksum error.
};

/**
 * @brief Returns a stable human-readable description of a filesystem error code.
 *
 * @param[in] error Filesystem error code to describe.
 *
 * @return Null-terminated static error-description string.
 */
const char *errorToString(Error error);

} // namespace fs
