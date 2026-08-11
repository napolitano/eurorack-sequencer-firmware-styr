/**
 * @file File.cpp
 * @brief Implements the File component used by the shared filesystem support.
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
#include "File.h"

#include "core/utils/ObjectPool.h"

#include "os/os.h"
#include "os/LockGuard.h"

namespace fs {

static ObjectPool<FIL, 2> filePool;
static os::Mutex filePoolMutex;

FIL *File::allocateFile() {
    os::LockGuard lock(filePoolMutex);
    FIL *file = filePool.allocate();
    ASSERT(file, "no free files");
    return file;
}

void File::releaseFile(FIL *file) {
    os::LockGuard lock(filePoolMutex);
    filePool.release(file);
}

} // namespace fs
