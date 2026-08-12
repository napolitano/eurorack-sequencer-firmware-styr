/**
 * @file TestEngineTransportClock.cpp
 * @brief Implements the TestEngineTransportClock component used by the unit test.
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

    void setResetInput(bool high) {
        _simulator.setDio(1, high);
    }

    void setClockInput(bool high) {
        _simulator.setDio(0, high);
    }

    void pulseClockInput() {
        // A valid external pulse is a low->high->low edge sequence.
        setClockInput(true);
        setClockInput(false);
    }

    void waitMs(int ms) {
        _simulator.wait(ms);
    }


private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = [this] () {
            _app.reset(new SequencerApp());
        };
        target.destroy = [this] () {
            _app.reset();
        };
        target.update = [this] () {
            _app->update();
        };
        return target;
    }

    std::unique_ptr<SequencerApp> _app;
    sim::Simulator _simulator;
};

static void forceMasterClockMode(SequencerApp &app) {
    auto &clockSetup = app.model.project().clockSetup();
    clockSetup.setMode(ClockSetup::Mode::Master);
    app.engine.update();
}

static void configureSlaveClockInputMode(SequencerApp &app, ClockSetup::ClockInputMode mode) {
    auto &clockSetup = app.model.project().clockSetup();
    clockSetup.setMode(ClockSetup::Mode::Slave);
    clockSetup.setClockInputMode(mode);
    // Ensure the updated setup is applied before input edges are simulated.
    app.engine.update();
}

static void configureSlaveClockInputMode(SequencerHarness &harness, ClockSetup::ClockInputMode mode) {
    configureSlaveClockInputMode(harness.app(), mode);
    harness.setResetInput(false);
    harness.setClockInput(false);
}

} // namespace

void test_01_clockstart_enters_running_state_in_master_mode() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        forceMasterClockMode(harness.app());
        engine.clockStart();
        engine.update();

        TEST_ASSERT_TRUE(engine.clockRunning());
    
}

void test_02_clockreset_stops_running_state_after_explicit_start() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        forceMasterClockMode(harness.app());
        engine.clockStart();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        engine.clockReset();
        engine.update();

        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_03_shift_pause_toggle_alternates_between_stop_and_continue() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        forceMasterClockMode(harness.app());
        clockSetup.setShiftMode(ClockSetup::ShiftMode::Pause);
        engine.update();

        engine.clockStart();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        engine.togglePlay(true);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        engine.togglePlay(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());
    
}

void test_04_clock_input_divisor_change_is_consumed_by_engine_update() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.clearDirty();
        TEST_ASSERT_FALSE(clockSetup.isDirty());

        clockSetup.setClockInputDivisor(16);
        TEST_ASSERT_TRUE(clockSetup.isDirty());

        engine.update();
        TEST_ASSERT_FALSE(clockSetup.isDirty());
    
}

void test_05_clock_input_mode_run_follows_reset_input_level_for_stop_and_continue() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureSlaveClockInputMode(harness.app(), ClockSetup::ClockInputMode::Run);

        // High reset input in Run mode means continue/running.
        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Repeating the same high level must not change running state.
        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Low reset input in Run mode means stop.
        harness.setResetInput(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        // Repeating the same low level must keep the engine stopped.
        harness.setResetInput(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_06_clock_input_mode_startstop_starts_on_high_and_stops_on_low() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureSlaveClockInputMode(harness.app(), ClockSetup::ClockInputMode::StartStop);

        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Repeating high in StartStop mode must keep running state stable.
        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.setResetInput(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        // Repeating low in StartStop mode must keep stopped state stable.
        harness.setResetInput(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_07_clock_input_mode_reset_starts_clock_on_first_pulse_while_reset_is_low() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Reset);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        harness.pulseClockInput();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());
    
}

void test_08_external_clock_pulse_advances_sequencer_ticks_in_reset_mode() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Reset);
        uint32_t tickBefore = engine.tick();

        harness.pulseClockInput();
        harness.waitMs(300);

        uint32_t tickAfter = engine.tick();
        TEST_ASSERT_TRUE(tickAfter > tickBefore);
    
}

void test_09_run_mode_resume_allows_subsequent_clock_pulses_to_progress_ticks() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Run);

        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        harness.pulseClockInput();
        harness.waitMs(300);
        uint32_t tickWhileRunning = engine.tick();
        TEST_ASSERT_TRUE(tickWhileRunning > 0);

        harness.setResetInput(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        uint32_t tickBeforeResumePulse = engine.tick();
        harness.pulseClockInput();
        harness.waitMs(300);
        TEST_ASSERT_TRUE(engine.tick() > tickBeforeResumePulse);
    
}

void test_10_updateclocksetup_reset_mode_stops_a_running_clock_when_reset_is_high() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Run);
        harness.setResetInput(true);
        engine.update();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Dirty transition: Run -> Reset while reset input remains high.
        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Reset);
        engine.update();
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_11_updateclocksetup_run_mode_handles_high_idle_and_low_running_branches() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        // Branch: reset high + idle -> slaveContinue.
        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Reset);
        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Run);
        engine.update();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Branch: reset low + running -> slaveStop.
        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Reset);
        harness.setResetInput(false);
        harness.pulseClockInput();
        engine.update();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Run);
        engine.update();
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_12_updateclocksetup_startstop_mode_handles_high_idle_and_low_running_branches() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        // Branch: reset high + idle -> slaveStart.
        configureSlaveClockInputMode(harness, ClockSetup::ClockInputMode::Reset);
        harness.setResetInput(true);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::StartStop);
        engine.update();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Branch: reset low + running -> slaveReset.
        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::Reset);
        harness.setResetInput(false);
        harness.pulseClockInput();
        engine.update();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        clockSetup.setClockInputMode(ClockSetup::ClockInputMode::StartStop);
        engine.update();
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_clockstart_enters_running_state_in_master_mode);
    RUN_TEST(test_02_clockreset_stops_running_state_after_explicit_start);
    RUN_TEST(test_03_shift_pause_toggle_alternates_between_stop_and_continue);
    RUN_TEST(test_04_clock_input_divisor_change_is_consumed_by_engine_update);
    RUN_TEST(test_05_clock_input_mode_run_follows_reset_input_level_for_stop_and_continue);
    RUN_TEST(test_06_clock_input_mode_startstop_starts_on_high_and_stops_on_low);
    RUN_TEST(test_07_clock_input_mode_reset_starts_clock_on_first_pulse_while_reset_is_low);
    RUN_TEST(test_08_external_clock_pulse_advances_sequencer_ticks_in_reset_mode);
    RUN_TEST(test_09_run_mode_resume_allows_subsequent_clock_pulses_to_progress_ticks);
    RUN_TEST(test_10_updateclocksetup_reset_mode_stops_a_running_clock_when_reset_is_high);
    RUN_TEST(test_11_updateclocksetup_run_mode_handles_high_idle_and_low_running_branches);
    RUN_TEST(test_12_updateclocksetup_startstop_mode_handles_high_idle_and_low_running_branches);
    return UNITY_END();
}



