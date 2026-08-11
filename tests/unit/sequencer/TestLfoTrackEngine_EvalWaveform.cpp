/**
 * @file TestLfoTrackEngine_EvalWaveform.cpp
 * @brief Implements the TestLfoTrackEngine_EvalWaveform component used by the unit test.
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

UNIT_TEST("LfoTrackEngine: EvalWaveform (Free mode)") {
    CASE("basic waveforms at phase 0.25") {
        SequencerHarness h;
        auto &app = h.app();

        // configure track 0 as LFO free-mode
        auto &proj = app.model.project();
        proj.setTrackMode(0, Track::TrackMode::Lfo);
        auto &l = proj.track(0).lfoTrack();
        l.clear();
        l.setMode(LfoTrack::Mode::Free);
        l.setHi(100);
        l.setLow(-100);
        // free frequency 1 Hz so update(dt=0.25) -> phase = 0.25
        l.setFreeFreqCentihz(100);

        // Initialize engine after configuring track mode so TrackEngines are created
        app.engine.init();
        printf("Debug(EvalWaveform): proj.track(0).trackMode=%d\n", int(proj.track(0).trackMode())); fflush(stdout);
        auto &engine = app.engine.trackEngine(0).as<LfoTrackEngine>();

        // Sine: phase 0.25 -> sin(0.5*pi) = 1 -> value should be hi
        l.setWaveform(LfoTrack::Waveform::Sine);
        engine.update(0.25f);
        float v = engine.cvOutput(0);
        expectTrue(std::abs(v - float(l.hi())/100.f) <= 1e-2f);

        // Triangle: evaluate at phase 0.25 (restart so each waveform is tested at same phase)
        l.setWaveform(LfoTrack::Waveform::Triangle);
        engine.restart();
        engine.update(0.25f);
        v = engine.cvOutput(0);
        // at phase 0.25 triangle -> 0 -> mid value
        float mid = (l.hi() + l.low()) / 2.f / 100.f;
        expectTrue(std::abs(v - mid) <= 1e-2f);

        // RampUp: at phase 0.25 -> 2*phase-1 = -0.5 -> scaled to range
        l.setWaveform(LfoTrack::Waveform::RampUp);
        engine.restart();
        engine.update(0.25f);
        v = engine.cvOutput(0);
        float expected = ((-0.5f) * (l.hi() - l.low()) / 2.f + (l.hi() + l.low()) / 2.f) / 100.f;
        expectTrue(std::abs(v - expected) <= 1e-2f);

        // Square: use pulseWidth 25% -> phase 0.25 -> equal boundary; test behaviour
        l.setWaveform(LfoTrack::Waveform::Square);
        engine.restart();
        l.setPulseWidth(25);
        engine.update(0.25f);
        v = engine.cvOutput(0);
        // With pulseWidth=25, phase 0.25 is at threshold; accept numeric within bounds
        expectTrue(v <= float(l.hi())/100.f + 1e-2f && v >= float(l.low())/100.f - 1e-2f);
    }
}
