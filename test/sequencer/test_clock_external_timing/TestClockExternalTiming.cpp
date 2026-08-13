/**
 * @file TestClockExternalTiming.cpp
 * @brief Implements the TestClockExternalTiming component used by the unit test.
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
#include <unity.h>

#include "SequencerApp.h"

#include "core/Simulator.h"

#include <memory>

namespace {

class SequencerHarness {
public:
    SequencerHarness() :
        _simulator(makeTarget())
    {
        _simulator.reboot();
    }

    SequencerApp &app() { return *_app; }

    void setResetInput(bool high) { _simulator.setDio(1, high); }
    void setClockInput(bool high) { _simulator.setDio(0, high); }
    void waitMs(int ms) { _simulator.wait(ms); }

    void pulseClockInput() {
        setClockInput(true);
        setClockInput(false);
    }

private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = [this] () { _app.reset(new SequencerApp()); };
        target.destroy = [this] () { _app.reset(); };
        target.update = [this] () { _app->update(); };
        return target;
    }

    std::unique_ptr<SequencerApp> _app;
    sim::Simulator _simulator;
};

static void configureExternalResetMode(SequencerHarness &harness, ClockSetup::Mode mode, int inputDivisor = 12) {
    auto &clockSetup = harness.app().model.project().clockSetup();
    clockSetup.setMode(mode);
    clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Reset);
    clockSetup.setClockInputDivisor(inputDivisor);

    harness.setResetInput(false);
    harness.setClockInput(false);
    harness.app().engine.update();
}

static void pulseAfter(SequencerHarness &harness, int intervalMs) {
    harness.waitMs(intervalMs);
    harness.pulseClockInput();
}

static void runIntervals(SequencerHarness &harness, const int *intervalsMs, size_t count) {
    harness.pulseClockInput();
    for (size_t i = 0; i < count; ++i) {
        pulseAfter(harness, intervalsMs[i]);
    }
}

static void assertTempoNear(float actual, float expected, float tolerance) {
    TEST_ASSERT_TRUE(actual >= expected - tolerance);
    TEST_ASSERT_TRUE(actual <= expected + tolerance);
}

} // namespace

void test_01_auto_mode_stops_after_locked_external_clock_times_out() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Auto);
        harness.pulseClockInput();
        pulseAfter(harness, 125);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());
        assertTempoNear(engine.tempo(), 120.f, 1.0f);

        harness.waitMs(700);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_02_auto_mode_re_locks_when_external_pulses_return_after_timeout() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Auto);
        harness.pulseClockInput();
        pulseAfter(harness, 125);
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.waitMs(700);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        harness.pulseClockInput();
        pulseAfter(harness, 125);
        engine.update();

        TEST_ASSERT_TRUE(engine.clockRunning());
        assertTempoNear(engine.tempo(), 120.f, 1.0f);
    
}

void test_03_slave_mode_detects_120_bpm_from_realistic_external_clock() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        const int intervalsMs[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, intervalsMs, sizeof(intervalsMs) / sizeof(intervalsMs[0]));

        TEST_ASSERT_TRUE(engine.clockRunning());
        assertTempoNear(engine.tempo(), 120.f, 0.75f);

}

void test_04_second_edge_is_already_a_valid_tempo_measurement() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        harness.app().model.project().setTempo(60.f);
        harness.app().engine.update();
        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        harness.pulseClockInput();
        harness.waitMs(125);
        harness.pulseClockInput();
        harness.app().engine.update();

        // The old timestamp-zero sentinel discarded this measurement and kept
        // reporting the 60 BPM acquisition seed until a third edge arrived.
        assertTempoNear(engine.tempo(), 120.f, 1.0f);

}

void test_05_realistic_jitter_stays_centered_on_120_bpm() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        const int intervalsMs[] = {
            123, 127, 124, 126, 122, 128, 125, 125,
            126, 124, 127, 123, 125, 125
        };
        runIntervals(harness, intervalsMs, sizeof(intervalsMs) / sizeof(intervalsMs[0]));

        TEST_ASSERT_TRUE(engine.clockRunning());
        assertTempoNear(engine.tempo(), 120.f, 1.5f);

}

void test_06_alternating_swing_intervals_do_not_bias_detected_bpm() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        // 100 ms + 150 ms averages to 125 ms per external pulse: 120 BPM at
        // the default four pulses per quarter note.
        const int intervalsMs[] = {
            100, 150, 100, 150, 100, 150, 100, 150, 100, 150
        };
        runIntervals(harness, intervalsMs, sizeof(intervalsMs) / sizeof(intervalsMs[0]));

        TEST_ASSERT_TRUE(engine.clockRunning());
        assertTempoNear(engine.tempo(), 120.f, 1.5f);

}

void test_07_large_tempo_change_relocks_within_a_few_external_edges() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        const int steady120Ms[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, steady120Ms, sizeof(steady120Ms) / sizeof(steady120Ms[0]));
        assertTempoNear(engine.tempo(), 120.f, 1.0f);

        // 166/167 ms at four pulses per quarter note is approximately 90 BPM.
        const int transition90Ms[] = { 167, 166, 167, 167 };
        for (int interval : transition90Ms) {
            pulseAfter(harness, interval);
        }

        assertTempoNear(engine.tempo(), 90.f, 4.0f);

}

void test_08_bpm_detection_is_consistent_across_external_clock_divisors() {
        struct Case {
            int divisor;
            int intervals[6];
        };

        const Case cases[] = {
            { 6,  { 62, 63, 62, 63, 62, 63 } },   // 8 PPQN external
            { 12, { 125, 125, 125, 125, 125, 125 } }, // 4 PPQN external
            { 24, { 250, 250, 250, 250, 250, 250 } }, // 2 PPQN external
            { 48, { 500, 500, 500, 500, 500, 500 } }, // 1 PPQN external
        };

        for (const auto &testCase : cases) {
            SequencerHarness harness;
            configureExternalResetMode(harness, ClockSetup::Mode::Slave, testCase.divisor);
            runIntervals(harness, testCase.intervals, 6);
            assertTempoNear(harness.app().engine.tempo(), 120.f, 1.5f);
        }

}

void test_09_auto_mode_timeout_scales_with_low_ppqn_external_clock() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        // A divisor of 48 is one external pulse per quarter note. At 100 BPM
        // valid pulses are 600 ms apart, so the historical fixed 500 ms timeout
        // incorrectly declared the clock dead between every pulse.
        configureExternalResetMode(harness, ClockSetup::Mode::Auto, 48);

        harness.pulseClockInput();
        harness.waitMs(600);
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.pulseClockInput();
        harness.waitMs(600);
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.pulseClockInput();
        assertTempoNear(engine.tempo(), 100.f, 2.0f);

}

void test_10_auto_mode_allows_slow_low_ppqn_clock_to_acquire_before_timeout() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        // One pulse per quarter note at 30 BPM is a two-second interval. The
        // acquisition timeout must not be derived from the unrelated project
        // tempo before that second edge has provided a real period.
        configureExternalResetMode(harness, ClockSetup::Mode::Auto, 48);

        harness.pulseClockInput();
        harness.waitMs(2000);
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.pulseClockInput();
        assertTempoNear(engine.tempo(), 30.f, 1.0f);
    
}

void test_11_auto_mode_still_times_out_after_multiple_missing_periods() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Auto, 48);

        harness.pulseClockInput();
        harness.waitMs(500);
        harness.pulseClockInput();
        harness.waitMs(500);
        harness.pulseClockInput();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // The measured pulse period is 500 ms, so the adaptive timeout is
        // 1.5 seconds. Missing more than that must still release Auto mode.
        harness.waitMs(1600);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

}


void test_12_extreme_alternating_swing_stays_centered_on_mean_bpm() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        // This is close to the simulator's maximum long/short swing while the
        // pair still averages to 125 ms per pulse (120 BPM at 4 PPQN).
        const int intervalsMs[] = { 63, 187, 63, 187, 63, 187, 63, 187 };
        runIntervals(harness, intervalsMs, sizeof(intervalsMs) / sizeof(intervalsMs[0]));

        assertTempoNear(engine.tempo(), 120.f, 1.5f);
}

void test_13_spurious_early_edge_does_not_corrupt_detected_bpm_or_gain_tick_credit() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);
        const int steady120Ms[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, steady120Ms, sizeof(steady120Ms) / sizeof(steady120Ms[0]));
        assertTempoNear(engine.tempo(), 120.f, 1.0f);

        uint32_t tickBefore = engine.tick();
        pulseAfter(harness, 1);   // impossible double-trigger/noise edge
        pulseAfter(harness, 124); // next real 125 ms edge

        assertTempoNear(engine.tempo(), 120.f, 1.0f);
        uint32_t advanced = engine.tick() - tickBefore;
        TEST_ASSERT_TRUE(advanced >= 45 && advanced <= 51);
}

void test_14_single_missing_external_edge_does_not_halve_detected_bpm() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);
        const int steady120Ms[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, steady120Ms, sizeof(steady120Ms) / sizeof(steady120Ms[0]));
        assertTempoNear(engine.tempo(), 120.f, 1.0f);

        pulseAfter(harness, 250); // one 125 ms pulse is missing
        assertTempoNear(engine.tempo(), 120.f, 1.0f);
        pulseAfter(harness, 125);
        assertTempoNear(engine.tempo(), 120.f, 1.0f);
}

void test_15_two_consistent_long_periods_confirm_a_real_large_slowdown() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);
        const int steady120Ms[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, steady120Ms, sizeof(steady120Ms) / sizeof(steady120Ms[0]));

        pulseAfter(harness, 250);
        assertTempoNear(engine.tempo(), 120.f, 1.0f); // first long edge is only a candidate
        pulseAfter(harness, 250);
        assertTempoNear(engine.tempo(), 60.f, 1.0f);
}

void test_16_two_consistent_short_periods_confirm_a_real_large_speedup() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);
        const int steady120Ms[] = { 125, 125, 125, 125, 125, 125 };
        runIntervals(harness, steady120Ms, sizeof(steady120Ms) / sizeof(steady120Ms[0]));

        pulseAfter(harness, 50);
        assertTempoNear(engine.tempo(), 120.f, 1.0f); // first short edge is only a candidate
        pulseAfter(harness, 50);
        assertTempoNear(engine.tempo(), 300.f, 2.0f);
}


void test_17_detected_slave_tempo_does_not_overwrite_project_master_tempo() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        project.setTempo(100.f);
        engine.update();
        configureExternalResetMode(harness, ClockSetup::Mode::Slave, 12);

        harness.pulseClockInput();
        pulseAfter(harness, 125);
        engine.update();

        assertTempoNear(engine.tempo(), 120.f, 1.0f);
        assertTempoNear(project.tempo(), 100.f, 0.01f);

        // Repeated engine updates must continue to configure the independent
        // master tempo without pulling the active slave BPM back to 100.
        for (int i = 0; i < 20; ++i) {
            harness.waitMs(1);
            engine.update();
        }

        assertTempoNear(engine.tempo(), 120.f, 1.0f);
        assertTempoNear(project.tempo(), 100.f, 0.01f);
}

void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_auto_mode_stops_after_locked_external_clock_times_out);
    RUN_TEST(test_02_auto_mode_re_locks_when_external_pulses_return_after_timeout);
    RUN_TEST(test_03_slave_mode_detects_120_bpm_from_realistic_external_clock);
    RUN_TEST(test_04_second_edge_is_already_a_valid_tempo_measurement);
    RUN_TEST(test_05_realistic_jitter_stays_centered_on_120_bpm);
    RUN_TEST(test_06_alternating_swing_intervals_do_not_bias_detected_bpm);
    RUN_TEST(test_07_large_tempo_change_relocks_within_a_few_external_edges);
    RUN_TEST(test_08_bpm_detection_is_consistent_across_external_clock_divisors);
    RUN_TEST(test_09_auto_mode_timeout_scales_with_low_ppqn_external_clock);
    RUN_TEST(test_10_auto_mode_allows_slow_low_ppqn_clock_to_acquire_before_timeout);
    RUN_TEST(test_11_auto_mode_still_times_out_after_multiple_missing_periods);
    RUN_TEST(test_12_extreme_alternating_swing_stays_centered_on_mean_bpm);
    RUN_TEST(test_13_spurious_early_edge_does_not_corrupt_detected_bpm_or_gain_tick_credit);
    RUN_TEST(test_14_single_missing_external_edge_does_not_halve_detected_bpm);
    RUN_TEST(test_15_two_consistent_long_periods_confirm_a_real_large_slowdown);
    RUN_TEST(test_16_two_consistent_short_periods_confirm_a_real_large_speedup);
    RUN_TEST(test_17_detected_slave_tempo_does_not_overwrite_project_master_tempo);
    return UNITY_END();
}

