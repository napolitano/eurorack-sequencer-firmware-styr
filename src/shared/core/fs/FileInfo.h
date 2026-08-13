/**
 * @file FileInfo.h
 * @brief Declares the FileInfo component used by the shared filesystem support.
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
 * @brief Exposes metadata for a filesystem entry returned by FatFs.
 */
class FileInfo {
public:
    /**
     * @brief Returns the name.
     *
     * @return Pointer to the name; `nullptr` when no value is available.
     */
    const char *name() const { return _info.fname; }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const { return _info.fsize; }

private:
    FILINFO _info; ///< FatFs metadata record backing this file-information wrapper.

    friend class Directory;
    /**
     * @brief Returns stat.
     *
     * @note Includes an unnamed `const char *` input parameter as declared by the inherited/interface signature.
     * @note Includes an unnamed `FileInfo &` input parameter as declared by the inherited/interface signature.
     *
     * @return Filesystem status/error code returned by the stat operation.
     */
    friend Error stat(const char *, FileInfo &);
};

} // namespace fs
