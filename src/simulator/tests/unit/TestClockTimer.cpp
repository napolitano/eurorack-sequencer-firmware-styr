/**
 * @file TestClockTimer.cpp
 * @brief Verifies simulator ClockTimer elapsed-time accounting.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include "UnitTest.h"

#include "core/Simulator.h"
#include "drivers/ClockTimer.h"

namespace {

class TimerListener : public ClockTimer::Listener {
public:
    void onClockTimerTick() override { ++count; }
    int count = 0;
};

} // namespace

UNIT_TEST("ClockTimer") {

    CASE("freshly enabled timer accounts for the full wait interval") {
        sim::Simulator simulator({
            [] () {},
            [] () {},
            [] () {}
        });

        ClockTimer timer;
        TimerListener listener;
        timer.setListener(&listener);
        timer.setPeriod(1000); // 1 ms
        timer.enable();

        simulator.wait(125);
        expectEqual(listener.count, 125);
    }

    CASE("re-enabled timer does not lose its first millisecond") {
        sim::Simulator simulator({
            [] () {},
            [] () {},
            [] () {}
        });

        ClockTimer timer;
        TimerListener listener;
        timer.setListener(&listener);
        timer.setPeriod(1000); // 1 ms

        timer.enable();
        simulator.wait(10);
        expectEqual(listener.count, 10);

        timer.disable();
        simulator.wait(700);
        expectEqual(listener.count, 10);

        timer.enable();
        simulator.wait(125);
        expectEqual(listener.count, 135);
    }
}
