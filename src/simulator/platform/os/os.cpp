/**
 * @file os.cpp
 * @brief Implements the os component used by the simulator OS integration.
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
#include "os.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace os {
namespace {

struct UpdateCallbackEntry {
    UpdateCallbackId id;
    UpdateCallback callback;
};

std::vector<UpdateCallbackEntry> &updateCallbackEntries() {
    static std::vector<UpdateCallbackEntry> callbacks;
    return callbacks;
}

UpdateCallbackId &nextUpdateCallbackId() {
    static UpdateCallbackId id = 1;
    return id;
}

} // namespace

UpdateCallbackId addUpdateCallback(UpdateCallback callback) {
    const auto id = nextUpdateCallbackId()++;
    updateCallbackEntries().push_back({ id, std::move(callback) });
    return id;
}

void removeUpdateCallback(UpdateCallbackId id) {
    auto &callbacks = updateCallbackEntries();
    callbacks.erase(
        std::remove_if(
            callbacks.begin(),
            callbacks.end(),
            [id] (const UpdateCallbackEntry &entry) { return entry.id == id; }),
        callbacks.end());
}

void runUpdateCallbacks() {
    auto &callbacks = updateCallbackEntries();

    // Resolve callbacks by stable id immediately before invocation. A callback
    // may destroy an object that owns another PeriodicTask, so direct vector
    // iteration would otherwise be invalidated by RAII deregistration.
    std::vector<UpdateCallbackId> ids;
    ids.reserve(callbacks.size());
    for (const auto &entry : callbacks) {
        ids.push_back(entry.id);
    }

    for (const auto id : ids) {
        const auto it = std::find_if(
            callbacks.begin(),
            callbacks.end(),
            [id] (const UpdateCallbackEntry &entry) { return entry.id == id; });
        if (it != callbacks.end()) {
            const auto callback = it->callback;
            callback();
        }
    }
}

} // namespace os
