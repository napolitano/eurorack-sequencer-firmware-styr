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

/**
 * @brief Generates the internal master clock, locks to external clock sources and drives clock/reset/run outputs.
 */
class Clock : private ClockTimer::Listener {
public:
    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode {
        Auto, ///< Selects the auto mode.
        Master, ///< Selects the master mode.
        Slave, ///< Selects the slave mode.
    };

    /**
     * @brief Enumerates the supported event values.
     */
    enum Event {
        Start       = (1<<0), ///< Selects the start event.
        Stop        = (1<<1), ///< Selects the stop event.
        Continue    = (1<<2), ///< Selects the continue event.
        Reset       = (1<<3), ///< Selects the reset event.
    };

    /**
     * @brief Stores output state used while the component is running.
     */
    struct OutputState {
        /**
         * @brief Whether clock is true in the current state.
         */
        bool clock = false; ///< Current logical level of the generated clock output.
        /**
         * @brief Whether reset is true in the current state.
         */
        bool reset = true; ///< Current logical level of the generated reset output.
        /**
         * @brief Whether run is true in the current state.
         */
        bool run = false; ///< Current logical level of the generated run output.
    };

    /**
     * @brief Defines callbacks emitted by the surrounding component.
     */
    struct Listener {
        /**
         * @brief Handles the clock output callback.
         *
         * @param[in] state New state or logical level to apply.
         */
        virtual void onClockOutput(const OutputState &state) = 0;
        /**
         * @brief Handles the clock midi callback.
         *
         * @note Includes an unnamed `uint8_t` input parameter as declared by the inherited/interface signature.
         */
        virtual void onClockMidi(uint8_t) = 0;
    };

    /**
     * @brief Constructs a Clock instance.
     *
     * @param[in] timer Timer instance supplying the timing source for this component.
     */
    Clock(ClockTimer &timer);

    /**
     * @brief Sets the listener.
     *
     * @param[in] listener Listener that receives callbacks; `nullptr` detaches the listener where supported.
     */
    void setListener(Listener *listener);

    /**
     * @brief Initializes the Clock and its runtime resources.
     */
    void init();

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const { return _mode; }
    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode);

    /**
     * @brief Returns the active mode.
     *
     * @return Configured active mode.
     */
    Mode activeMode() const;

    /**
     * @brief Reports whether idle.
     *
     * @return `true` if idle; otherwise `false`.
     */
    bool isIdle() const { return _state == State::Idle; }
    /**
     * @brief Reports whether running.
     *
     * @return `true` if running; otherwise `false`.
     */
    bool isRunning() const { return _state != State::Idle; }

    /**
     * @brief Returns the ppqn.
     *
     * @return Sequencer clock resolution in pulses per quarter note (PPQN).
     */
    int ppqn() const { return _ppqn; }
    /**
     * @brief Returns the bpm.
     *
     * @return Tempo in beats per minute.
     */
    float bpm() const { return _state == State::SlaveRunning ? _slaveBpm : _masterBpm; }
    /**
     * @brief Returns the current absolute sequencer engine tick.
     *
     * @return Current absolute sequencer engine tick.
     */
    uint32_t tick() const { return _tick; }
    /**
     * @brief Returns the tick duration.
     *
     * @return Duration of one sequencer engine tick, in seconds.
     */
    float tickDuration() const { return 60.f / (bpm() * _ppqn); }

    // Master clock control
    /**
     * @brief Starts the internal master clock.
     */
    void masterStart();
    /**
     * @brief Stops the internal master clock.
     */
    void masterStop();
    /**
     * @brief Continues the internal master clock without resetting musical position.
     */
    void masterContinue();
    /**
     * @brief Requests a master-clock reset.
     */
    void masterReset();

    /**
     * @brief Returns the master bpm.
     *
     * @return Tempo in beats per minute.
     */
    float masterBpm() const { return _masterBpm; }
    /**
     * @brief Sets the master bpm.
     *
     * @param[in] bpm Tempo in beats per minute.
     */
    void setMasterBpm(float bpm);

    // Slave clock control
    /**
     * @brief Configures one external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     * @param[in] enabled `true` to enable the behavior; `false` to disable it.
     */
    void slaveConfigure(int slave, int divisor, bool enabled);
    /**
     * @brief Processes one pulse from an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     */
    void slaveTick(int slave);
    /**
     * @brief Processes a start event from an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     */
    void slaveStart(int slave);
    /**
     * @brief Processes a stop event from an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     */
    void slaveStop(int slave);
    /**
     * @brief Processes a continue event from an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     */
    void slaveContinue(int slave);
    /**
     * @brief Processes a reset event from an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     */
    void slaveReset(int slave);
    /**
     * @brief Processes a MIDI realtime message for an external clock source.
     *
     * @param[in] slave Zero-based external clock source index.
     * @param[in] msg Raw MIDI message byte to process.
     */
    void slaveHandleMidi(int slave, uint8_t msg);

    // Clock output
    /**
     * @brief Configures the hardware clock output divisor and pulse width.
     *
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     * @param[in] pulse Clock-output pulse width in the model-defined unit.
     */
    void outputConfigure(int divisor, int pulse);
    /**
     * @brief Configures swing for the hardware clock output.
     *
     * @param[in] swing Swing setting in the model-defined range.
     */
    void outputConfigureSwing(int swing);
    /**
     * @brief Returns the current clock/reset/run output state.
     *
     * @return Reference to the output state.
     */
    const OutputState &outputState() const { return _outputState; }

    // Sequencer interface
    /**
     * @brief Returns and clears pending clock transport events.
     *
     * @return Bit mask containing pending transport events; zero indicates that no event is pending.
     */
    Event checkEvent();
    /**
     * @brief Returns the next pending sequencer tick, if available.
     *
     * @param[out] tick Absolute sequencer engine tick at which the operation is evaluated.
     *
     * @return `true` when a pending sequencer tick was returned; `false` when no tick is available.
     */
    bool checkTick(uint32_t *tick);

private:
    /**
     * @brief Enumerates the supported state values.
     */
    enum class State {
        Idle, ///< Selects the idle state.
        // Running,
        MasterRunning, ///< Selects the master running state.
        SlaveRunning, ///< Selects the slave running state.
    };

    // ClockTimer::Listener
    /**
     * @brief Handles the clock timer tick callback.
     */
    void onClockTimerTick();

    /**
     * @brief Clears generated and consumed clock tick counters.
     */
    void resetTicks();
    /**
     * @brief Queues a clock start event for the sequencer.
     */
    void requestStart();
    /**
     * @brief Queues a clock stop event for the sequencer.
     */
    void requestStop();
    /**
     * @brief Queues a clock continue event for the sequencer.
     */
    void requestContinue();
    /**
     * @brief Queues a clock reset event for the sequencer.
     */
    void requestReset();
    /**
     * @brief Adds a transport event to the pending clock-event mask.
     *
     * @param[in] event Event to process.
     */
    void requestEvent(Event event);

    /**
     * @brief Applies a new runtime state and performs the transition-specific bookkeeping.
     *
     * @param[in] state New state or logical level to apply.
     */
    void setState(State state);

    /**
     * @brief Programs the timer for the current internal master tempo.
     */
    void setupMasterTimer();
    /**
     * @brief Programs the timer used for external-clock interpolation.
     */
    void setupSlaveTimer();

    /**
     * @brief Clears external-clock acquisition and filtering state.
     */
    void resetSlaveTiming();
    /**
     * @brief Clears learned short/long external-clock swing timing.
     */
    void resetSlaveSwingTracking();
    /**
     * @brief Stores a confirmed short/long swing period pair for phase prediction.
     *
     * @param[in] aUs First measured period in microseconds.
     * @param[in] bUs Second measured period in microseconds.
     * @param[in] nextExpectedUs Expected duration of the next external-clock interval, in microseconds.
     */
    void setSlaveSwingPair(uint32_t aUs, uint32_t bUs, uint32_t nextExpectedUs);
    /**
     * @brief Updates external-clock period estimation from a newly measured interval.
     *
     * @param[in] periodUs Measured or configured period in microseconds.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void updateSlaveTiming(uint32_t periodUs, uint32_t divisor);
    /**
     * @brief Accepts a measured external-clock period as the current timing lock.
     *
     * @param[in] periodUs Measured or configured period in microseconds.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void lockSlaveTiming(uint32_t periodUs, uint32_t divisor);
    /**
     * @brief Computes an initial external-clock period from the configured master tempo.
     *
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     *
     * @return Slave seed period us, in microseconds.
     */
    uint32_t slaveSeedPeriodUs(uint32_t divisor) const;
    /**
     * @brief Computes the timeout used to decide that an external clock disappeared.
     *
     * @return Slave timeout us, in microseconds.
     */
    uint32_t slaveTimeoutUs() const;
    /**
     * @brief Performs a wrap-safe comparison between microsecond timestamps.
     *
     * @param[in] now Current timestamp in the same wrap-around time domain as the compared target.
     * @param[in] target Target microsecond timestamp in the same wrapping 32-bit timer domain.
     *
     * @return `true` if time reached; otherwise `false`.
     */
    static bool timeReached(uint32_t now, uint32_t target);
    /**
     * @brief Checks whether two periods are within the supplied relative tolerance.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     * @param[in] b Right/second value used by the comparison or binary operation.
     * @param[in] tolerancePercent Maximum relative difference, in percent, accepted by the comparison.
     *
     * @return `true` if periods similar; otherwise `false`.
     */
    static bool periodsSimilar(uint32_t a, uint32_t b, uint32_t tolerancePercent);

    /**
     * @brief Emits a MIDI realtime/clock message to the configured listener.
     *
     * @param[in] msg Raw MIDI message byte to process.
     */
    void outputMidiMessage(uint8_t msg);
    /**
     * @brief Updates clock outputs for the supplied engine tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     */
    void outputTick(uint32_t tick);
    /**
     * @brief Changes the hardware clock-output state.
     *
     * @param[in] clock Whether clock is enabled for this operation.
     */
    void outputClock(bool clock);
    /**
     * @brief Changes the reset-output state.
     *
     * @param[in] reset Whether reset is enabled for this operation.
     */
    void outputReset(bool reset);
    /**
     * @brief Changes the run-output state.
     *
     * @param[in] run Whether run is enabled for this operation.
     */
    void outputRun(bool run);

    /**
     * @brief Returns slave enabled.
     *
     * @param[in] slave Zero-based external clock source index.
     *
     * @return `true` if slave enabled; otherwise `false`.
     */
    bool slaveEnabled(int slave) const { return _slaves[slave].enabled; }

    /**
     * @brief Slave timer period constant used by this component.
     */
    static constexpr uint32_t SlaveTimerPeriod = 100; ///< Service period of the external-clock interpolation timer, in microseconds.
    /**
     * @brief Slave min timeout us, in microseconds.
     */
    static constexpr uint32_t SlaveMinTimeoutUs = 500000; ///< Minimum time without an external pulse before Auto mode may release slave lock, in microseconds.
    /**
     * @brief Slave acquisition timeout us, in microseconds.
     */
    static constexpr uint32_t SlaveAcquisitionTimeoutUs = 3000000; ///< Timeout allowed while waiting for the first complete external-clock period measurement, in microseconds.
    /**
     * @brief Slave stable filter time us, in microseconds.
     */
    static constexpr uint32_t SlaveStableFilterTimeUs = 350000; ///< Time constant used to smooth an already stable external-clock period estimate, in microseconds.
    /**
     * @brief Slave change filter time us, in microseconds.
     */
    static constexpr uint32_t SlaveChangeFilterTimeUs = 75000; ///< Shorter time constant used while converging on a confirmed tempo change, in microseconds.
    /**
     * @brief Slave short transition percent, in percent.
     */
    static constexpr uint32_t SlaveShortTransitionPercent = 60; ///< Period ratio below which a sample is treated as a possible faster-tempo transition, in percent of the current estimate.
    /**
     * @brief Slave long transition percent, in percent.
     */
    static constexpr uint32_t SlaveLongTransitionPercent = 140; ///< Period ratio above which a sample is treated as a possible slower-tempo transition, in percent of the current estimate.
    /**
     * @brief Slave transition match percent, in percent.
     */
    static constexpr uint32_t SlaveTransitionMatchPercent = 20; ///< Maximum relative difference between consecutive transition candidates before a tempo change is confirmed, in percent.
    /**
     * @brief Slave swing pair tolerance percent, in percent.
     */
    static constexpr uint32_t SlaveSwingPairTolerancePercent = 15; ///< Maximum relative error allowed when recognizing a repeated short/long swing-period pair, in percent.
    /**
     * @brief Slave swing tracking tolerance percent, in percent.
     */
    static constexpr uint32_t SlaveSwingTrackingTolerancePercent = 15; ///< Maximum period error accepted while tracking an established swing pair, in percent.
    /**
     * @brief Slave swing min separation percent, in percent.
     */
    static constexpr uint32_t SlaveSwingMinSeparationPercent = 15; ///< Minimum short/long period separation required before timing is classified as swing, in percent.
    /**
     * @brief Slave max supported bpm, in beats per minute.
     */
    static constexpr float SlaveMaxSupportedBpm = 1000.f; ///< Maximum external tempo accepted as a plausible clock measurement, in beats per minute.
    /**
     * @brief Number of slave entries supported by the implementation.
     */
    static constexpr size_t SlaveCount = 4; ///< Number of independent external clock sources tracked by the clock engine.

    /**
     * @brief Listener that receives callbacks from this component.
     */
    Listener *_listener = nullptr; ///< Listener that receives callbacks from this component.

    /**
     * @brief Timer resource used to drive this component.
     */
    ClockTimer &_timer; ///< Timer resource used to drive this component.
    int _ppqn = CONFIG_PPQN; ///< Sequencer clock resolution in pulses per quarter note (PPQN).

    Mode _mode = Mode::Auto; ///< Configured clock mode; `activeMode()` may differ while Auto mode is locked to an external source.

    /**
     * @brief Configured internal master tempo in beats per minute.
     */
    float _masterBpm = 120.f; ///< Configured internal master tempo in beats per minute.

    /**
     * @brief Stores the fields required to represent slave.
     */
    struct Slave {
        /**
         * @brief Runtime value representing divisor.
         */
        int divisor; ///< Number of sequencer-engine ticks represented by one pulse for this clock source/output.
        /**
         * @brief Whether  is enabled.
         */
        bool enabled; ///< Whether this option/source is enabled.
    };
    /**
     * @brief Fixed-capacity storage for slaves.
     */
    std::array<Slave, SlaveCount> _slaves; ///< Runtime synchronization state for each supported external clock source.

    /**
     * @brief Stores output values produced by the enclosing operation or simulation step.
     */
    struct Output {
        /**
         * @brief Runtime value representing divisor.
         */
        int divisor; ///< Number of sequencer-engine ticks represented by one pulse for this clock source/output.
        /**
         * @brief Runtime value representing pulse.
         */
        int pulse; ///< Configured clock-output pulse width in the model-defined clock pulse unit.
        /**
         * @brief Runtime value representing swing.
         */
        int swing; ///< Swing percentage used by the simulated clock source; 50 means straight timing.
        /**
         * @brief Next tick scheduled or expected by this component.
         */
        uint32_t nextTick; ///< Absolute engine tick at which the next output clock cycle begins.
        /**
         * @brief Next tick on scheduled or expected by this component.
         */
        uint32_t nextTickOn; ///< Absolute engine tick at which the next output pulse transitions high.
        /**
         * @brief Next tick off scheduled or expected by this component.
         */
        uint32_t nextTickOff; ///< Absolute engine tick at which the current/next output pulse transitions low.
    };
    Output _output; ///< Scheduling/configuration state for the hardware clock output.
    OutputState _outputState; ///< Current clock/reset/run output timing state for all configured clock outputs.

    uint32_t _requestedEvents = Reset; ///< Bit mask of pending transport events waiting to be consumed by `checkEvent()`.
    State _state = State::Idle; ///< Current runtime state.

    /**
     * @brief Current absolute sequencer engine tick.
     */
    volatile uint32_t _tick; ///< Current absolute sequencer engine tick.
    /**
     * @brief Most recent sequencer engine tick consumed by the client.
     */
    volatile uint32_t _tickProcessed; ///< Most recent sequencer engine tick consumed by the client.

    /**
     * @brief Index of the active external clock source, or -1 when no source is active.
     */
    volatile int32_t _activeSlave = -1; ///< Index of the active external clock source, or -1 when no source is active.

    /**
     * @brief Accumulated service time in microseconds.
     */
    uint32_t _elapsedUs; ///< Accumulated service time in microseconds.
    /**
     * @brief Timestamp of the most recent accepted external-clock pulse, in microseconds.
     */
    uint32_t _lastSlaveTickUs; ///< Timestamp of the most recent accepted external-clock pulse, in microseconds.
    /**
     * @brief Whether a valid last slave tick measurement/value has been captured.
     */
    bool _haveLastSlaveTick = false; ///< Whether a valid last slave tick measurement/value has been captured.
    /**
     * @brief Previous slave period us, in microseconds.
     */
    uint32_t _previousSlavePeriodUs = 0; ///< Previous slave period us, in microseconds.
    /**
     * @brief Whether a valid previous slave period measurement/value has been captured.
     */
    bool _havePreviousSlavePeriod = false; ///< Whether a valid previous slave period measurement/value has been captured.
    /**
     * @brief Runtime value representing slave timing samples.
     */
    uint32_t _slaveTimingSamples = 0; ///< Number of accepted external-clock timing samples since the current lock/acquisition began.
    /**
     * @brief Suspect short period us, in microseconds.
     */
    uint32_t _suspectShortPeriodUs = 0; ///< Suspect short period us, in microseconds.
    /**
     * @brief Whether a valid suspect short period measurement/value has been captured.
     */
    bool _haveSuspectShortPeriod = false; ///< Whether a valid suspect short period measurement/value has been captured.
    /**
     * @brief Suspect long period us, in microseconds.
     */
    uint32_t _suspectLongPeriodUs = 0; ///< Suspect long period us, in microseconds.
    /**
     * @brief Whether a valid suspect long period measurement/value has been captured.
     */
    bool _haveSuspectLongPeriod = false; ///< Whether a valid suspect long period measurement/value has been captured.
    /**
     * @brief Whether slave swing tracking is true in the current state.
     */
    bool _slaveSwingTracking = false; ///< True after a stable short/long external-clock pair has been recognized for swing phase prediction.
    /**
     * @brief Slave swing short period us, in microseconds.
     */
    uint32_t _slaveSwingShortPeriodUs = 0; ///< Slave swing short period us, in microseconds.
    /**
     * @brief Slave swing long period us, in microseconds.
     */
    uint32_t _slaveSwingLongPeriodUs = 0; ///< Slave swing long period us, in microseconds.
    /**
     * @brief Slave expected swing period us, in microseconds.
     */
    uint32_t _slaveExpectedSwingPeriodUs = 0; ///< Slave expected swing period us, in microseconds.
    /**
     * @brief Slave tick period us, in microseconds.
     */
    uint32_t _slaveTickPeriodUs = 0; ///< Slave tick period us, in microseconds.
    /**
     * @brief Runtime value representing slave sub ticks pending.
     */
    uint32_t _slaveSubTicksPending; ///< Number of interpolated slave sub-ticks waiting to be emitted.
    /**
     * @brief Slave sub tick period us, in microseconds.
     */
    uint32_t _slaveSubTickPeriodUs = 0; ///< Slave sub tick period us, in microseconds.
    /**
     * @brief Timestamp at which the next interpolated slave sub-tick is due, in microseconds.
     */
    uint32_t _nextSlaveSubTickUs; ///< Timestamp at which the next interpolated slave sub-tick is due, in microseconds.

    /**
     * @brief Tempo estimated from the active external clock source, in beats per minute.
     */
    float _slaveBpm = 0.f; ///< Tempo estimated from the active external clock source, in beats per minute.
};
