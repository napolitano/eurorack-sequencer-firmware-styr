/**
 * @file TestLfoTrackEngine_Nudge.cpp
 * @brief Implements the TestLfoTrackEngine_Nudge component used by the unit test.
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

void test_01_lfotrackengine_nudge_shifts_phase_in_sync_mode() {
    // Single harness with two tracks to avoid global resource conflicts
    SequencerHarness h;
    auto &app = h.app();
    auto &proj = app.model.project();

    // configure track 0 (nudge=0)
    proj.setTrackMode(0, Track::TrackMode::Lfo);
    // configure track 1 (nudge=1)
    proj.setTrackMode(1, Track::TrackMode::Lfo);
    app.engine.init();

    auto &lfo0 = proj.track(0).lfoTrack();
    lfo0.clear();
    lfo0.setWaveform(LfoTrack::Waveform::Sine);
    lfo0.setMode(LfoTrack::Mode::Sync);
    lfo0.setHi(100); lfo0.setLow(-100);
    lfo0.setSpeed(16);
    lfo0.setNudge(0);

    auto &lfo1 = proj.track(1).lfoTrack();
    lfo1.clear();
    lfo1.setWaveform(LfoTrack::Waveform::Sine);
    lfo1.setMode(LfoTrack::Mode::Sync);
    lfo1.setHi(100); lfo1.setLow(-100);
    lfo1.setSpeed(16);
    lfo1.setNudge(1);

    // compute tick values: for speed=16, totalTicks = steps * seqDivisor * (PPQN/SEQ_PPQN)
    const int seqDivisor = 12;
    const int ticksPerStep = seqDivisor * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const int totalTicks = 16 * ticksPerStep;
    int tick0 = totalTicks * 1 / 4; // phase 0.25 -> idx = 4
    int tickShifted = totalTicks * 5 / 16; // corresponds to shifted idx 5 -> phase=5/16

    // Tick engines and compare outputs
    app.engine.trackEngine(0).tick(tickShifted);
    float v0 = app.engine.trackEngine(0).cvOutput(0);

    app.engine.trackEngine(1).tick(tick0);
    float v1 = app.engine.trackEngine(1).cvOutput(0);

    // v1 (nudge=1 at phase 0.25) should equal v0 (nudge=0 at phase 5/16)
    TEST_ASSERT_TRUE(std::abs(v0 - v1) <= 1e-3f);
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_lfotrackengine_nudge_shifts_phase_in_sync_mode);
    return UNITY_END();
}

