/**
 * @file Clock.h
 * @brief Declares the Clock component used by the sequencer engine.
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
#pragma once

#include "Config.h"

#include "drivers/ClockTimer.h"

#include <array>
#include <functional>

#include <cstddef>
#include <cstdint>

class Clock : private ClockTimer::Listener {
public:
    enum class Mode {
        Auto,
        Master,
        Slave,
    };

    enum Event {
        Start       = (1<<0),
        Stop        = (1<<1),
        Continue    = (1<<2),
        Reset       = (1<<3),
    };

    struct OutputState {
        bool clock = false;
        bool reset = true;
        bool run = false;
    };

    struct Listener {
        virtual void onClockOutput(const OutputState &state) = 0;
        virtual void onClockMidi(uint8_t) = 0;
    };

    Clock(ClockTimer &timer);

    void setListener(Listener *listener);

    void init();

    Mode mode() const { return _mode; }
    void setMode(Mode mode);

    Mode activeMode() const;

    bool isIdle() const { return _state == State::Idle; }
    bool isRunning() const { return _state != State::Idle; }

    int ppqn() const { return _ppqn; }
    float bpm() const { return _state == State::SlaveRunning ? _slaveBpm : _masterBpm; }
    uint32_t tick() const { return _tick; }
    float tickDuration() const { return 60.f / (bpm() * _ppqn); }

    // Master clock control
    void masterStart();
    void masterStop();
    void masterContinue();
    void masterReset();

    float masterBpm() const { return _masterBpm; }
    void setMasterBpm(float bpm);

    // Slave clock control
    void slaveConfigure(int slave, int divisor, bool enabled);
    void slaveTick(int slave);
    void slaveStart(int slave);
    void slaveStop(int slave);
    void slaveContinue(int slave);
    void slaveReset(int slave);
    void slaveHandleMidi(int slave, uint8_t msg);

    // Clock output
    void outputConfigure(int divisor, int pulse);
    void outputConfigureSwing(int swing);
    const OutputState &outputState() const { return _outputState; }

    // Sequencer interface
    Event checkEvent();
    bool checkTick(uint32_t *tick);

private:
    enum class State {
        Idle,
        // Running,
        MasterRunning,
        SlaveRunning,
    };

    // ClockTimer::Listener
    void onClockTimerTick();

    void resetTicks();
    void requestStart();
    void requestStop();
    void requestContinue();
    void requestReset();
    void requestEvent(Event event);

    void setState(State state);

    void setupMasterTimer();
    void setupSlaveTimer();

    void resetSlaveTiming();
    void resetSlaveSwingTracking();
    void setSlaveSwingPair(uint32_t aUs, uint32_t bUs, uint32_t nextExpectedUs);
    void updateSlaveTiming(uint32_t periodUs, uint32_t divisor);
    void lockSlaveTiming(uint32_t periodUs, uint32_t divisor);
    uint32_t slaveSeedPeriodUs(uint32_t divisor) const;
    uint32_t slaveTimeoutUs() const;
    static bool timeReached(uint32_t now, uint32_t target);
    static bool periodsSimilar(uint32_t a, uint32_t b, uint32_t tolerancePercent);

    void outputMidiMessage(uint8_t msg);
    void outputTick(uint32_t tick);
    void outputClock(bool clock);
    void outputReset(bool reset);
    void outputRun(bool run);

    bool slaveEnabled(int slave) const { return _slaves[slave].enabled; }

    static constexpr uint32_t SlaveTimerPeriod = 100; // us
    static constexpr uint32_t SlaveMinTimeoutUs = 500000; // us
    static constexpr uint32_t SlaveAcquisitionTimeoutUs = 3000000; // us
    static constexpr uint32_t SlaveStableFilterTimeUs = 350000; // us
    static constexpr uint32_t SlaveChangeFilterTimeUs = 75000; // us
    static constexpr uint32_t SlaveShortTransitionPercent = 60;
    static constexpr uint32_t SlaveLongTransitionPercent = 140;
    static constexpr uint32_t SlaveTransitionMatchPercent = 20;
    static constexpr uint32_t SlaveSwingPairTolerancePercent = 15;
    static constexpr uint32_t SlaveSwingTrackingTolerancePercent = 15;
    static constexpr uint32_t SlaveSwingMinSeparationPercent = 15;
    static constexpr float SlaveMaxSupportedBpm = 1000.f;
    static constexpr size_t SlaveCount = 4;

    Listener *_listener = nullptr;

    ClockTimer &_timer;
    int _ppqn = CONFIG_PPQN;

    Mode _mode = Mode::Auto;

    float _masterBpm = 120.f;

    struct Slave {
        int divisor;
        bool enabled;
    };
    std::array<Slave, SlaveCount> _slaves;

    struct Output {
        int divisor;
        int pulse;
        int swing;
        uint32_t nextTick;
        uint32_t nextTickOn;
        uint32_t nextTickOff;
    };
    Output _output;
    OutputState _outputState;

    uint32_t _requestedEvents = Reset;
    State _state = State::Idle;

    volatile uint32_t _tick;
    volatile uint32_t _tickProcessed;

    volatile int32_t _activeSlave = -1;

    uint32_t _elapsedUs;
    uint32_t _lastSlaveTickUs; // timestamp of the most recent external/MIDI clock pulse
    bool _haveLastSlaveTick = false;
    uint32_t _previousSlavePeriodUs = 0;
    bool _havePreviousSlavePeriod = false;
    uint32_t _slaveTimingSamples = 0;
    uint32_t _suspectShortPeriodUs = 0;
    bool _haveSuspectShortPeriod = false;
    uint32_t _suspectLongPeriodUs = 0;
    bool _haveSuspectLongPeriod = false;
    bool _slaveSwingTracking = false;
    uint32_t _slaveSwingShortPeriodUs = 0;
    uint32_t _slaveSwingLongPeriodUs = 0;
    uint32_t _slaveExpectedSwingPeriodUs = 0;
    uint32_t _slaveTickPeriodUs = 0; // filtered external clock pulse period
    uint32_t _slaveSubTicksPending; // number of slave sub ticks pending
    uint32_t _slaveSubTickPeriodUs = 0; // slave sub tick period time
    uint32_t _nextSlaveSubTickUs; // time of next slave sub tick

    float _slaveBpm = 0.f;
};
