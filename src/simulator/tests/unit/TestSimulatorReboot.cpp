/**
 * @file TestSimulatorReboot.cpp
 * @brief Verifies simulator target recreation does not retain stale driver observers.
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

#include "SequencerApp.h"
#include "core/Simulator.h"

#include <iostream>
#include <memory>

UNIT_TEST("SimulatorReboot") {

    CASE("removed simulator update callback is not invoked again") {
        std::cout << "[TestSimulatorReboot] standalone callback lifecycle" << std::endl;
        int callbackCount = 0;

        sim::Simulator simulator({
            [] () {},
            [] () {},
            [] () {}
        });

        const auto callbackId = simulator.addUpdateCallback([&callbackCount] () {
            ++callbackCount;
        });

        simulator.wait(2);
        expect(callbackCount == 2);

        simulator.removeUpdateCallback(callbackId);
        simulator.wait(2);
        expect(callbackCount == 2);
    }

    CASE("repeated reboot keeps simulator driver observers and callbacks valid") {
        std::unique_ptr<SequencerApp> sequencer;

        sim::Simulator simulator({
            [&sequencer] () { sequencer.reset(new SequencerApp()); },
            [&sequencer] () { sequencer.reset(); },
            [&sequencer] () { sequencer->update(); }
        });

        // The simulator-side file task is owned by SequencerApp. It is created
        // only after fs::Volume exists and is deregistered before that Volume is
        // destroyed, so target recreation cannot leak an OS callback across boots.
        std::cout << "[TestSimulatorReboot] initial target boot" << std::endl;
        simulator.wait(10);
        expect(sequencer.get() != nullptr);

        // Verify callback removal while the normal simulator target is alive.
        // This covers the API used by ClockTimer without creating an invalid
        // target-less simulator environment.
        int callbackCount = 0;
        const auto callbackId = simulator.addUpdateCallback([&callbackCount] () {
            ++callbackCount;
        });
        simulator.wait(2);
        expect(callbackCount == 2);
        simulator.removeUpdateCallback(callbackId);
        simulator.wait(2);
        expect(callbackCount == 2);
        std::cout << "[TestSimulatorReboot] callback removal verified" << std::endl;

        // Recreate the complete target repeatedly. This specifically guards the
        // observer and ClockTimer callback lifetimes that previously left stale
        // pointers/captures behind after Simulator::reboot().
        for (int iteration = 0; iteration < 12; ++iteration) {
            std::cout << "[TestSimulatorReboot] reboot " << iteration + 1 << "/12" << std::endl;
            simulator.reboot();
            expect(sequencer.get() != nullptr);
            simulator.wait(10);
            simulator.setButton(0, (iteration & 1) != 0);
            simulator.setAdc(0, float((iteration % 5) - 2));
            simulator.wait(2);
        }

        expect(sequencer.get() != nullptr);
        std::cout << "[TestSimulatorReboot] completed" << std::endl;
    }
}
