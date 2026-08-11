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

namespace os {

std::vector<std::function<void(void)>> &updateCallbacks() {
    static std::vector<std::function<void(void)>> callbacks;
    return callbacks;
}

} // namespace os
