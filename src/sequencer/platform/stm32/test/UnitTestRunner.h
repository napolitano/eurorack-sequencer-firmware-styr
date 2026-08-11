/**
 * @file UnitTestRunner.h
 * @brief Declares the UnitTestRunner component used by the STM32 sequencer platform.
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

#include "core/Debug.h"

#include "os/os.h"

#include "drivers/System.h"
#include "drivers/Console.h"
#include "drivers/HighResolutionTimer.h"

namespace test {

    bool run(const char *name);

} // namespace test

int runWrapper(const char *name) {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    static os::Task<16*1024> testTask("test", 0, [name] () {
        test::run(name);

        while (1) {}
    });

    os::startScheduler();

    return 0;
}

#define UNIT_TEST_RUNNER_PRINTF(_fmt_, ...) \
    dbg_printf(_fmt_, ##__VA_ARGS__)

#define UNIT_TEST_RUNNER(_name_)        \
    int main() {                        \
        return runWrapper(_name_);      \
    }                                   \

#define CURRENT_TIME() HighResolutionTimer::us()
