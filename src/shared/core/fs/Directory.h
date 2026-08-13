/**
 * @file Directory.h
 * @brief Declares the Directory component used by the shared filesystem support.
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

#include "Error.h"
#include "FileInfo.h"

#include "ff/ff.h"

#include <cstddef>
#include <cstdint>

namespace fs {

/**
 * @brief Wraps FatFs directory enumeration and exposes the current entry metadata.
 */
class Directory {
public:
    /**
     * @brief Constructs a Directory instance.
     *
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     */
    Directory(const char *path) {
        _error = Error(f_opendir(&_dir, path));
    }

    /**
     * @brief Destroys the Directory instance.
     */
    ~Directory() {
        f_closedir(&_dir);
    }

    /**
     * @brief Returns the info.
     *
     * @return Reference to the info.
     */
    const FileInfo &info() const { return _info; }

    /**
     * @brief Returns the next.
     *
     * @return `true` if next; otherwise `false`.
     */
    bool next() {
        _error = Error(f_readdir(&_dir, &_info._info));
        return _error == OK && _info.name()[0] != '\0';
    }

private:
    DIR _dir; ///< FatFs directory handle for the currently open directory enumeration.
    Error _error = OK; ///< Current/last error status.
    FileInfo _info; ///< File metadata buffer populated for the current directory entry.
};

} // namespace fs
