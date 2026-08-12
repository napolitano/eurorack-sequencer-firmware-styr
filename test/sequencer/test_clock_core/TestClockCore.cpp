/**
 * @file TestClockCore.cpp
 * @brief Implements the TestClockCore component used by the unit test.
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

#include "engine/Clock.h"

#include "core/Simulator.h"

#include <memory>
#include <vector>

namespace {

class RecordingListener : public Clock::Listener {
public:
    void onClockOutput(const Clock::OutputState &state) override {
        outputs.push_back(state);
    }

    void onClockMidi(uint8_t msg) override {
        midi.push_back(msg);
    }

    std::vector<Clock::OutputState> outputs;
    std::vector<uint8_t> midi;
};

class ClockHarness {
public:
    ClockHarness() :
        _simulator(makeTarget()),
        _clock(_timer)
    {
        _clock.init();
        _clock.outputConfigure(12, 1);
        _clock.outputConfigureSwing(0);
        _clock.setListener(&_listener);
    }

    Clock &clock() { return _clock; }
    RecordingListener &listener() { return _listener; }

    void waitMs(int ms) { _simulator.wait(ms); }

    void drainEvents() {
        while (_clock.checkEvent() != Clock::Event(0)) {
        }
    }

    int drainTicks() {
        uint32_t tick = 0;
        int count = 0;
        while (_clock.checkTick(&tick)) {
            ++count;
        }
        return count;
    }

private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = []() {};
        target.destroy = []() {};
        target.update = []() {};
        return target;
    }

    sim::Simulator _simulator;
    ClockTimer _timer;
    Clock _clock;
    RecordingListener _listener;
};

} // namespace

void test_01_setlistener_accepts_nullptr_and_no_output_callback_is_fired() {
        ClockHarness harness;

        harness.clock().setListener(nullptr);
        size_t outputsBefore = harness.listener().outputs.size();

        harness.clock().masterStart();
        harness.waitMs(20);

        TEST_ASSERT_TRUE((harness.listener().outputs.size()) == (outputsBefore));
    
}

void test_02_master_clock_emits_ticks_only_after_pending_events_are_consumed() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.masterStart();
        harness.waitMs(30);

        uint32_t tick = 0;
        TEST_ASSERT_FALSE(clock.checkTick(&tick)); // blocked by pending Start/Reset events

        harness.drainEvents();
        TEST_ASSERT_TRUE(clock.checkTick(&tick));
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));
    
}

void test_03_mode_switching_stops_currently_running_clock_source() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        harness.drainEvents();

        clock.slaveStart(0);
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));

        clock.setMode(Clock::Mode::Master);
        TEST_ASSERT_TRUE(clock.isIdle());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));

        harness.drainEvents();
        clock.masterStart();
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));

        clock.setMode(Clock::Mode::Slave);
        TEST_ASSERT_TRUE(clock.isIdle());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));
    
}

void test_04_master_control_calls_are_ignored_when_clock_mode_is_slave() {
        ClockHarness harness;
        auto &clock = harness.clock();

        harness.drainEvents();
        clock.setMode(Clock::Mode::Slave);

        clock.masterStart();
        clock.masterContinue();
        clock.masterReset();

        TEST_ASSERT_TRUE(clock.isIdle());
        TEST_ASSERT_TRUE((clock.checkEvent()) == (Clock::Event(0)));
    
}

void test_05_master_controls_are_ignored_while_slave_is_running() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.slaveStart(0);
        harness.drainEvents();

        clock.masterStart();
        clock.masterContinue();
        clock.masterReset();

        TEST_ASSERT_TRUE(clock.isRunning());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));
    
}

void test_06_slave_start_continue_reset_are_ignored_in_explicit_master_mode() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        harness.drainEvents();

        clock.setMode(Clock::Mode::Master);

        clock.slaveStart(0);
        clock.slaveContinue(0);
        clock.slaveReset(0);

        TEST_ASSERT_TRUE(clock.isIdle());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));
        TEST_ASSERT_TRUE((clock.checkEvent()) == (Clock::Event(0)));
    
}

void test_07_slavehandlemidi_routes_start_tick_continue_and_stop_messages() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(1, 12, true);
        harness.drainEvents();

        clock.slaveHandleMidi(1, MidiMessage::Start);
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));

        harness.waitMs(2);
        clock.slaveHandleMidi(1, MidiMessage::Tick);
        harness.waitMs(10);

        harness.drainEvents();
        TEST_ASSERT_TRUE(harness.drainTicks() > 0);

        clock.slaveHandleMidi(1, MidiMessage::Continue);
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));

        clock.slaveHandleMidi(1, MidiMessage::Stop);
        TEST_ASSERT_TRUE(clock.isIdle());

        // Non-realtime message should be ignored.
        clock.slaveHandleMidi(1, 0x00);
        TEST_ASSERT_TRUE(clock.isIdle());
    
}

void test_08_disabled_slave_source_ignores_all_slave_transport_calls() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(2, 12, false);
        harness.drainEvents();

        clock.slaveStart(2);
        clock.slaveTick(2);
        clock.slaveContinue(2);
        clock.slaveStop(2);
        clock.slaveReset(2);

        TEST_ASSERT_TRUE(clock.isIdle());
        TEST_ASSERT_TRUE((clock.checkEvent()) == (Clock::Event(0)));
    
}

void test_09_slave_guard_paths_keep_active_source_unchanged_when_wrong_slave_id_is_used() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.slaveConfigure(1, 12, true);
        clock.slaveStart(0);
        harness.drainEvents();

        // Running on slave 0: operations for slave 1 must be ignored.
        clock.slaveStart(1);
        clock.slaveStop(1);
        clock.slaveReset(1);

        TEST_ASSERT_TRUE(clock.isRunning());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));

        // Active source can still be stopped normally.
        clock.slaveStop(0);
        TEST_ASSERT_TRUE(clock.isIdle());
    
}

void test_10_slave_tick_is_ignored_when_source_is_inactive_or_not_active_slave() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.slaveConfigure(1, 12, true);
        harness.drainEvents();

        // Idle clock: ticks should not start anything.
        clock.slaveTick(0);
        TEST_ASSERT_TRUE(clock.isIdle());

        // Running on slave 0: ticks from slave 1 must be ignored.
        clock.slaveStart(0);
        harness.drainEvents();
        uint32_t tickBefore = clock.tick();
        clock.slaveTick(1);
        harness.waitMs(2);
        TEST_ASSERT_TRUE((clock.tick()) == (tickBefore));
    
}

void test_11_slave_transport_calls_are_ignored_while_master_clock_is_running() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.masterStart();
        harness.drainEvents();

        uint32_t tickBefore = clock.tick();

        clock.slaveStart(0);
        clock.slaveStop(0);
        clock.slaveReset(0);

        harness.waitMs(3);
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));
        TEST_ASSERT_TRUE(clock.isRunning());
        TEST_ASSERT_TRUE(clock.tick() >= tickBefore);
    
}

void test_12_avgslaveperiod_returns_zero_before_any_samples() {
        ClockHarness harness;
        auto &clock = harness.clock();

        TEST_ASSERT_TRUE((clock.avgSlavePeriod()) == (uint32_t(0)));
    
}

void test_13_slavestart_on_already_active_slave_keeps_slave_running() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.slaveStart(0);
        harness.drainEvents();

        clock.slaveStart(0);

        TEST_ASSERT_TRUE(clock.isRunning());
        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));
    
}

void test_14_slave_period_window_rollover_and_averaging_path_remain_stable() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.slaveConfigure(0, 12, true);
        clock.slaveStart(0);
        harness.drainEvents();

        for (int i = 0; i < 48; ++i) {
            harness.waitMs(2);
            clock.slaveTick(0);
        }

        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Slave));
        TEST_ASSERT_TRUE(clock.bpm() > 0.f);
    
}

void test_15_non_zero_swing_path_executes_during_master_output_scheduling() {
        ClockHarness harness;
        auto &clock = harness.clock();

        clock.outputConfigureSwing(50);
        clock.masterStart();
        harness.drainEvents();
        harness.waitMs(40);

        TEST_ASSERT_TRUE((clock.activeMode()) == (Clock::Mode::Master));
        TEST_ASSERT_TRUE(harness.listener().midi.size() > 0);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_setlistener_accepts_nullptr_and_no_output_callback_is_fired);
    RUN_TEST(test_02_master_clock_emits_ticks_only_after_pending_events_are_consumed);
    RUN_TEST(test_03_mode_switching_stops_currently_running_clock_source);
    RUN_TEST(test_04_master_control_calls_are_ignored_when_clock_mode_is_slave);
    RUN_TEST(test_05_master_controls_are_ignored_while_slave_is_running);
    RUN_TEST(test_06_slave_start_continue_reset_are_ignored_in_explicit_master_mode);
    RUN_TEST(test_07_slavehandlemidi_routes_start_tick_continue_and_stop_messages);
    RUN_TEST(test_08_disabled_slave_source_ignores_all_slave_transport_calls);
    RUN_TEST(test_09_slave_guard_paths_keep_active_source_unchanged_when_wrong_slave_id_is_used);
    RUN_TEST(test_10_slave_tick_is_ignored_when_source_is_inactive_or_not_active_slave);
    RUN_TEST(test_11_slave_transport_calls_are_ignored_while_master_clock_is_running);
    RUN_TEST(test_12_avgslaveperiod_returns_zero_before_any_samples);
    RUN_TEST(test_13_slavestart_on_already_active_slave_keeps_slave_running);
    RUN_TEST(test_14_slave_period_window_rollover_and_averaging_path_remain_stable);
    RUN_TEST(test_15_non_zero_swing_path_executes_during_master_output_scheduling);
    return UNITY_END();
}



