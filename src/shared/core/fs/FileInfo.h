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

class FileInfo {
public:
    const char *name() const { return _info.fname; }

    size_t size() const { return _info.fsize; }

private:
    FILINFO _info;

    friend class Directory;
    friend Error stat(const char *, FileInfo &);
};

} // namespace fs
