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

static uint32_t runPulseTrain(SequencerHarness &harness, int pulses, int pulseSpacingMs) {
    auto &engine = harness.app().engine;
    for (int i = 0; i < pulses; ++i) {
        harness.pulseClockInput();
        harness.waitMs(pulseSpacingMs);
    }
    return engine.tick();
}

} // namespace

void test_01_auto_mode_stops_after_external_pulses_time_out() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Auto);
        harness.pulseClockInput();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.waitMs(700);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_02_auto_mode_re_locks_when_external_pulses_return_after_timeout() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Auto);
        harness.pulseClockInput();
        harness.waitMs(40);
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.waitMs(700);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        uint32_t tickBeforeRelock = engine.tick();
        harness.pulseClockInput();
        harness.waitMs(60);
        engine.update();

        TEST_ASSERT_TRUE(engine.clockRunning());
        // Relock may restart the tick base, but it must produce a different tick state.
        TEST_ASSERT_TRUE(engine.tick() != tickBeforeRelock);
    
}

void test_03_slave_mode_keeps_running_stable_with_moderate_pulse_jitter() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureExternalResetMode(harness, ClockSetup::Mode::Slave);

        const int pulseIntervalsMs[] = { 20, 24, 19, 23, 21, 22, 20, 24 };
        harness.pulseClockInput();
        harness.waitMs(30);

        uint32_t tickBefore = engine.tick();
        for (int interval : pulseIntervalsMs) {
            harness.pulseClockInput();
            harness.waitMs(interval);
            TEST_ASSERT_TRUE(engine.clockRunning());
        }

        uint32_t tickAfter = engine.tick();
        TEST_ASSERT_TRUE(tickAfter > tickBefore);
        TEST_ASSERT_TRUE(engine.tempo() > 30.f);
        TEST_ASSERT_TRUE(engine.tempo() < 300.f);
    
}

void test_04_higher_clock_input_divisor_yields_more_internal_ticks_per_pulse_train() {
        uint32_t lowDivTicks = 0;
        {
            SequencerHarness lowDivHarness;
            configureExternalResetMode(lowDivHarness, ClockSetup::Mode::Slave, 6);
            lowDivTicks = runPulseTrain(lowDivHarness, 6, 40);
        }

        uint32_t midDivTicks = 0;
        {
            SequencerHarness midDivHarness;
            configureExternalResetMode(midDivHarness, ClockSetup::Mode::Slave, 12);
            midDivTicks = runPulseTrain(midDivHarness, 6, 40);
        }

        uint32_t highDivTicks = 0;
        {
            SequencerHarness highDivHarness;
            configureExternalResetMode(highDivHarness, ClockSetup::Mode::Slave, 24);
            highDivTicks = runPulseTrain(highDivHarness, 6, 40);
        }

        TEST_ASSERT_TRUE(lowDivTicks > 0);
        TEST_ASSERT_TRUE(midDivTicks > lowDivTicks);
        TEST_ASSERT_TRUE(highDivTicks > midDivTicks);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_auto_mode_stops_after_external_pulses_time_out);
    RUN_TEST(test_02_auto_mode_re_locks_when_external_pulses_return_after_timeout);
    RUN_TEST(test_03_slave_mode_keeps_running_stable_with_moderate_pulse_jitter);
    RUN_TEST(test_04_higher_clock_input_divisor_yields_more_internal_ticks_per_pulse_train);
    return UNITY_END();
}



