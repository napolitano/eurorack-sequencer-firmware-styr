/**
 * @file TestLfoTrackEngine_FreeMode.cpp
 * @brief Implements the TestLfoTrackEngine_FreeMode component used by the unit test.
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
#include <cstdio>
#include <unity.h>
#include "SequencerApp.h"
#include "core/Simulator.h"
#include <memory>
#include <cmath>

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

void test_01_lfotrackengine_free_mode_frequency_and_output() {
    SequencerHarness harness;
    auto &app = harness.app();
    // configure track 0 as LFO free-mode
    auto &project = app.model.project();
    project.setTrackMode(0, Track::TrackMode::Lfo);
    app.engine.init();
    auto &lfo = project.track(0).lfoTrack();
    lfo.clear();
    lfo.setWaveform(LfoTrack::Waveform::Sine);
    lfo.setMode(LfoTrack::Mode::Free);
    lfo.setHi(100);   // +1.00V
    lfo.setLow(-100); // -1.00V
    lfo.setFreeFreqCentihz(200); // 2.00 Hz

    auto &engine = app.engine;
    // start from phase 0, advance by dt = 0.25s -> phase = 0.5 -> sin(pi)=0
    engine.trackEngine(0).update(0.25f);
    float v = engine.trackEngine(0).cvOutput(0);
    TEST_ASSERT_TRUE(std::abs(v) <= 1e-3f);
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_lfotrackengine_free_mode_frequency_and_output);
    return UNITY_END();
}


