/**
 * @file TestLfoTrackEngine_RandomSteps.cpp
 * @brief Implements the TestLfoTrackEngine_RandomSteps component used by the unit test.
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
#include "UnitTest.h"

#include "SequencerApp.h"
#include "core/Simulator.h"
#include "model/LfoTrack.h"

namespace {
class SequencerHarness {
public:
    SequencerHarness() : _simulator(makeTarget()) { _simulator.reboot(); }
    SequencerApp &app() { return *_app; }
private:
    sim::Target makeTarget() {
        sim::Target t;
        t.create  = [this] () { _app.reset(new SequencerApp()); };
        t.destroy = [this] () { _app.reset(); };
        t.update  = [this] () { _app->update(); };
        return t;
    }
    std::unique_ptr<SequencerApp> _app;
    sim::Simulator _simulator;
};
}

UNIT_TEST("LfoTrackEngine: Random steps generation") {
    CASE("generate preview for Free mode (64 steps)") {
        SequencerHarness h;
        auto &app = h.app();
        auto &proj = app.model.project();
        proj.setTrackMode(0, Track::TrackMode::Lfo);
        auto &l = proj.track(0).lfoTrack();
        l.clear();
        l.setMode(LfoTrack::Mode::Free);

        // Initialize engine after configuring track mode so TrackEngines are created
        app.engine.init();
        auto &engine = app.engine.trackEngine(0).as<LfoTrackEngine>();
        engine.generateRandomStepsIfNeeded();
        auto const &steps = engine.randomSteps();
        expectEqual(int(steps.size()), 64);
    }

    CASE("generate steps for Sync mode honors speed") {
        SequencerHarness h;
        auto &app = h.app();
        auto &proj = app.model.project();
        proj.setTrackMode(0, Track::TrackMode::Lfo);
        auto &l = proj.track(0).lfoTrack();
        l.clear();
        l.setMode(LfoTrack::Mode::Sync);
        l.setSpeed(10);

        // Ensure engine has been initialized so track engines are created
        app.engine.init();
        printf("Debug: trackMode after init = %d\n", int(proj.track(0).trackMode())); fflush(stdout);
        auto &engine = app.engine.trackEngine(0).as<LfoTrackEngine>();
        engine.reset();
        // generateRandomStepsIfNeeded will call ensureRandomSteps when mode==Sync
        engine.generateRandomStepsIfNeeded();
        auto const &steps = engine.randomSteps();
        expectEqual(int(steps.size()), 10);
    }
}




