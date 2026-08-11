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

#include <memory>

UNIT_TEST("SimulatorReboot") {

    CASE("repeated reboot keeps simulator driver observers valid") {
        std::unique_ptr<SequencerApp> sequencer;

        sim::Simulator simulator({
            [&sequencer] () { sequencer.reset(new SequencerApp()); },
            [&sequencer] () { sequencer.reset(); },
            [&sequencer] () { sequencer->update(); }
        });

        // First step creates the target. Repeated target destruction and
        // recreation used to leave dangling simulator-driver observers behind.
        simulator.wait(10);
        for (int iteration = 0; iteration < 12; ++iteration) {
            simulator.reboot();
            simulator.wait(10);
            simulator.setButton(0, (iteration & 1) != 0);
            simulator.setAdc(0, float((iteration % 5) - 2));
            simulator.wait(2);
        }

        expect(sequencer.get() != nullptr);
    }
}
