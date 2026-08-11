/**
 * @file FileSystem.h
 * @brief Declares the FileSystem component used by the shared filesystem support.
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
#include "Volume.h"
#include "File.h"
#include "Directory.h"

#include <cstddef>
#include <cstdint>

namespace fs {

void setVolume(Volume *volume);
Volume &volume();

Error mkdir(const char *path);
Error rmdir(const char *path);
Error remove(const char *path);
Error rename(const char *oldPath, const char *newPath);

bool exists(const char *path);

} // namespace fs
