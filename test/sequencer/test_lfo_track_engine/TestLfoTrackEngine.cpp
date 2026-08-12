/**
 * @file TestLfoTrackEngine.cpp
 * @brief Implements the TestLfoTrackEngine component used by the unit test.
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
int _early_debug = (printf("TestLfoTrackEngine: file loaded\n"), fflush(stdout), 0);

#include <unity.h>
#include "SequencerApp.h"
#include "core/Simulator.h"
#include <memory>
#include <cassert>
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

static LfoTrackEngine &configureLfoTrack(SequencerApp &app, int trackIndex) {
    printf("configureLfoTrack: start\n"); fflush(stdout);
    auto &project = app.model.project();
    project.setTrackMode(trackIndex, Track::TrackMode::Lfo);
    printf("configureLfoTrack: setTrackMode done\n"); fflush(stdout);
    app.engine.init(); // Ensure track engines are recreated for new mode
    printf("configureLfoTrack: engine.init() done\n"); fflush(stdout);
    auto &track = project.track(trackIndex);
    printf("configureLfoTrack: got track, mode=%d\n", int(track.trackMode())); fflush(stdout);
    auto &lfoTrack = track.lfoTrack();
    printf("configureLfoTrack: got lfoTrack at %p\n", (void*)&lfoTrack); fflush(stdout);
    lfoTrack.clear();
    lfoTrack.setWaveform(LfoTrack::Waveform::Sine);
    lfoTrack.setHi(1000);
    lfoTrack.setLow(-1000);
    lfoTrack.setMode(LfoTrack::Mode::Sync);
    lfoTrack.setSpeed(16);
    lfoTrack.setClip(false);
    lfoTrack.setPulseWidth(50);
    printf("configureLfoTrack: lfoTrack configured\n"); fflush(stdout);
    auto &engineRef = app.engine.trackEngine(trackIndex);
    assert(&engineRef != nullptr && "trackEngine is null");
    printf("configureLfoTrack: got engineRef\n"); fflush(stdout);
    return engineRef.as<LfoTrackEngine>();
}

} // namespace

void test_01_lfotrackengine_basic_sine_output() {
    printf("TestLfoTrackEngine: start\n"); fflush(stdout);
    SequencerHarness harness;
    printf("TestLfoTrackEngine: harness created\n"); fflush(stdout);
    auto &app = harness.app();
    printf("TestLfoTrackEngine: got app\n"); fflush(stdout);
    auto &engine = app.engine;
    printf("TestLfoTrackEngine: got engine\n"); fflush(stdout);
    auto &lfo = configureLfoTrack(app, 0);
    printf("TestLfoTrackEngine: got lfo\n"); fflush(stdout);
    assert(&lfo != nullptr && "LfoTrackEngine is null");
    float last = 0.f;
    for (int i = 0; i < 32; ++i) {
        printf("Tick %d\n", i); fflush(stdout);
        engine.trackEngine(0).tick(i);
        printf("After tick %d\n", i); fflush(stdout);
        float v = engine.trackEngine(0).cvOutput(0);
        printf("After cvOutput %d: %f\n", i, v); fflush(stdout);
        // LFO CV outputs are in volts (approximately -5..+5V), not normalized -1..1
        TEST_ASSERT_TRUE(std::abs(v) <= 5.1f);
        last = v;
    }
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_lfotrackengine_basic_sine_output);
    return UNITY_END();
}

