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

/**
 * @brief Sets the filesystem volume used by the global filesystem facade.
 *
 * @param[in] volume Value consumed by the documented operation.
 */
void setVolume(Volume *volume);
/**
 * @brief Returns the filesystem volume currently used by the global filesystem facade.
 *
 * @return Reference to the currently configured filesystem volume.
 */
Volume &volume();

/**
 * @brief Creates a directory at the supplied path.
 *
 * @param[in] path Null-terminated filesystem path.
 *
 * @return Computed result in the domain described by this function.
 */
Error mkdir(const char *path);
/**
 * @brief Removes an empty directory at the supplied path.
 *
 * @param[in] path Null-terminated filesystem path.
 *
 * @return Computed result in the domain described by this function.
 */
Error rmdir(const char *path);
/**
 * @brief Removes a file at the supplied path.
 *
 * @param[in] path Null-terminated filesystem path.
 *
 * @return Computed result in the domain described by this function.
 */
Error remove(const char *path);
/**
 * @brief Renames or moves a filesystem entry.
 *
 * @param[in] oldPath Existing filesystem path to rename or move.
 * @param[in] newPath Destination filesystem path/name.
 *
 * @return Computed result in the domain described by this function.
 */
Error rename(const char *oldPath, const char *newPath);

/**
 * @brief Reports whether a filesystem entry exists at the supplied path.
 *
 * @param[in] path Null-terminated filesystem path.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
bool exists(const char *path);

} // namespace fs
