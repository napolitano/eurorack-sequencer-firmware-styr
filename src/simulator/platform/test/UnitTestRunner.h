/**
 * @file UnitTestRunner.h
 * @brief Declares the UnitTestRunner component used by the simulator platform.
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
#include "drivers/HighResolutionTimer.h"

#include "core/Debug.h"

namespace test {

    bool run(const char *name);

} // namespace test

#define UNIT_TEST_RUNNER_PRINTF(_fmt_, ...) \
    dbg_printf(_fmt_, ##__VA_ARGS__)

#define UNIT_TEST_RUNNER(_name_)        \
int main() {                            \
    HighResolutionTimer::init();        \
    return test::run(_name_) ? 0 : 1;   \
}                                       \

#define CURRENT_TIME() HighResolutionTimer::us()
