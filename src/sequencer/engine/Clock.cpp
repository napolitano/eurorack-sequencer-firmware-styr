/**
 * @file Clock.cpp
 * @brief Implements the Clock component used by the sequencer engine.
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
#include "Clock.h"

#include "Groove.h"

#include "os/os.h"
#include "core/Debug.h"
#include "core/math/Math.h"
#include "core/midi/MidiMessage.h"
#include "drivers/ClockTimer.h"

#include <algorithm>
#include <cmath>

Clock::Clock(ClockTimer &timer) :
    _timer(timer)
{
    resetTicks();

    _timer.setListener(this);
}

void Clock::init() {
    _timer.disable();
}

void Clock::setListener(Listener *listener) {
    os::InterruptLock lock;
    _listener = listener;
    if (_listener) {
        _listener->onClockOutput(_outputState);
    }
}

void Clock::setMode(Mode mode) {
    if (mode != _mode) {
        if (mode == Mode::Master && _state == State::SlaveRunning) {
            slaveStop(_activeSlave);
        }
        if (mode == Mode::Slave && _state == State::MasterRunning) {
            masterStop();
        }
        _mode = mode;
    }
}

Clock::Mode Clock::activeMode() const {
    switch (_state) {
    case State::MasterRunning:
        return Mode::Master;
    case State::SlaveRunning:
        return Mode::Slave;
    default:
        return _mode;
    }
}

void Clock::masterStart() {
    os::InterruptLock lock;

    if (_state == State::SlaveRunning || _mode == Mode::Slave) {
        return;
    }

    setState(State::MasterRunning);
    requestStart();
    resetTicks();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::masterStop() {
    os::InterruptLock lock;

    if (_state != State::MasterRunning) {
        return;
    }

    setState(State::Idle);
    requestStop();

    _timer.disable();
}

void Clock::masterContinue() {
    os::InterruptLock lock;

    if (_state != State::Idle || _mode == Mode::Slave) {
        return;
    }

    setState(State::MasterRunning);
    requestContinue();

    _timer.disable();
    setupMasterTimer();
    _timer.enable();
}

void Clock::masterReset() {
    os::InterruptLock lock;

    if (_state == State::SlaveRunning || _mode == Mode::Slave) {
        return;
    }

    setState(State::Idle);
    requestReset();

    _timer.disable();
}

void Clock::setMasterBpm(float bpm) {
    os::InterruptLock lock;

    // Engine::update() runs every millisecond. Avoid rewriting the hardware
    // timer ARR continuously while the requested tempo is unchanged.
    if (std::fabs(bpm - _masterBpm) < 0.0001f) {
        return;
    }

    _masterBpm = bpm;
    if (_state == State::MasterRunning) {
        setupMasterTimer();
    }
}

void Clock::slaveConfigure(int slave, int divisor, bool enabled) {
    os::InterruptLock lock;

    // Disabling the currently active source must also leave SlaveRunning.
    // Otherwise subsequent slaveTick()/slaveReset() calls are rejected by the
    // disabled-source guard and Auto mode can remain stuck on a source that can
    // no longer produce clock events.
    bool disableActiveSlave =
        !enabled &&
        _state == State::SlaveRunning &&
        _activeSlave == slave;

    _slaves[slave] = { std::max(1, divisor), enabled };

    if (disableActiveSlave) {
        setState(State::Idle);
        _activeSlave = -1;
        resetSlaveTiming();
        requestStop();
        _timer.disable();
    }
}

void Clock::slaveTick(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state != State::SlaveRunning || _activeSlave != slave) {
        return;
    }

    uint32_t divisor = uint32_t(std::max(1, _slaves[slave].divisor));
    uint32_t pulseCredits = 1;
    uint32_t measuredPeriodUs = 0;
    uint32_t nextInterpolationPeriodUs = 0;
    bool haveMeasurement = false;
    bool lockMeasurement = false;
    bool establishSwingPair = false;
    uint32_t swingPairAUs = 0;
    uint32_t swingPairBUs = 0;
    uint32_t swingNextExpectedUs = 0;
    bool preserveCurrentRawPeriod = false;
    bool regularPairCandidate = false;
    uint32_t regularPairPreviousUs = 0;
    uint32_t regularPairCurrentUs = 0;
    uint32_t regularPairMeanBeforeUs = 0;
    uint32_t regularPairTimingSamples = 0;

    if (_slaveTickPeriodUs == 0) {
        _slaveTickPeriodUs = slaveSeedPeriodUs(divisor);
    }

    // Use an explicit validity flag instead of `_lastSlaveTickUs > 0`.
    // External Reset mode starts at timestamp zero, so the old sentinel threw
    // away the second edge and delayed the first real BPM measurement until
    // the third pulse.
    if (_haveLastSlaveTick) {
        uint32_t periodUs = _elapsedUs - _lastSlaveTickUs;

        if (periodUs > 0) {
            bool handledAsTrackedSwing = false;

            // BPM and external phase are separate concerns. Once an alternating
            // long/short pair has been established, accept the predicted next
            // swing interval even when it is far outside the mean-period
            // transition thresholds. This lets the 192-PPQN interpolation
            // follow the source's swung pulse phase instead of merely reporting
            // the correct average BPM while drifting behind every short edge.
            if (_slaveSwingTracking &&
                periodsSimilar(periodUs, _slaveExpectedSwingPeriodUs, SlaveSwingTrackingTolerancePercent)) {
                measuredPeriodUs = periodUs;
                haveMeasurement = true;
                handledAsTrackedSwing = true;

                bool expectedShort =
                    _slaveExpectedSwingPeriodUs == _slaveSwingShortPeriodUs;
                _slaveExpectedSwingPeriodUs = expectedShort
                    ? _slaveSwingLongPeriodUs
                    : _slaveSwingShortPeriodUs;
                nextInterpolationPeriodUs = _slaveExpectedSwingPeriodUs;
            } else if (_slaveSwingTracking) {
                // The source no longer follows the learned long/short pattern.
                // Fall back to normal transition/noise classification and let a
                // new pair establish itself if swing is still present.
                resetSlaveSwingTracking();
            }

            if (!handledAsTrackedSwing) {
                if (_haveSuspectShortPeriod) {
                    // A very early edge is ambiguous: it may be a genuine large
                    // tempo increase or a double-trigger/noise edge. Two similar
                    // short periods confirm the tempo change. Otherwise merge the
                    // suspect edge with the following interval, reconstructing the
                    // period from the last accepted clock edge.
                    if (periodsSimilar(_suspectShortPeriodUs, periodUs, SlaveTransitionMatchPercent)) {
                        measuredPeriodUs = uint32_t(
                            (uint64_t(_suspectShortPeriodUs) + periodUs) / 2
                        );
                        haveMeasurement = true;
                        lockMeasurement = true;
                        pulseCredits = 2;
                    } else {
                        uint32_t combinedPeriodUs = _suspectShortPeriodUs + periodUs;
                        uint32_t pairAverageUs = combinedPeriodUs / 2;
                        uint32_t currentUs = std::max<uint32_t>(1, _slaveTickPeriodUs);

                        if (periodsSimilar(pairAverageUs, currentUs, SlaveSwingPairTolerancePercent)) {
                            // A short/long pair whose mean matches the established
                            // period is swing, not a double trigger. Both received
                            // edges are real and therefore contribute tick credit.
                            measuredPeriodUs = pairAverageUs;
                            pulseCredits = 2;
                            establishSwingPair = true;
                            swingPairAUs = _suspectShortPeriodUs;
                            swingPairBUs = periodUs;
                            swingNextExpectedUs = _suspectShortPeriodUs;
                            nextInterpolationPeriodUs = swingNextExpectedUs;
                            preserveCurrentRawPeriod = true;
                        } else {
                            // Otherwise the early edge was noise. Merge across it
                            // to recover the period between the surrounding real
                            // clock edges and do not grant it clock credit.
                            measuredPeriodUs = combinedPeriodUs;
                            pulseCredits = 1;
                        }
                        haveMeasurement = true;
                    }
                    _suspectShortPeriodUs = 0;
                    _haveSuspectShortPeriod = false;
                } else if (_haveSuspectLongPeriod) {
                    // One unusually long period is much more likely to be a
                    // dropped external edge than an intentional large slowdown.
                    // Require a second similar long period before changing tempo.
                    if (periodsSimilar(_suspectLongPeriodUs, periodUs, SlaveTransitionMatchPercent)) {
                        measuredPeriodUs = uint32_t(
                            (uint64_t(_suspectLongPeriodUs) + periodUs) / 2
                        );
                        haveMeasurement = true;
                        lockMeasurement = true;
                    } else {
                        uint32_t pairAverageUs = uint32_t(
                            (uint64_t(_suspectLongPeriodUs) + periodUs) / 2
                        );
                        uint32_t currentUs = std::max<uint32_t>(1, _slaveTickPeriodUs);

                        if (periodsSimilar(pairAverageUs, currentUs, SlaveSwingPairTolerancePercent)) {
                            // Complementary long/short intervals are a swung pair.
                            measuredPeriodUs = pairAverageUs;
                            establishSwingPair = true;
                            swingPairAUs = _suspectLongPeriodUs;
                            swingPairBUs = periodUs;
                            swingNextExpectedUs = _suspectLongPeriodUs;
                            nextInterpolationPeriodUs = swingNextExpectedUs;
                            preserveCurrentRawPeriod = true;
                        } else {
                            // A single long interval followed by the established
                            // cadence is treated as a dropped/missing edge rather
                            // than as a tempo change.
                            measuredPeriodUs = periodUs;
                        }
                        haveMeasurement = true;
                    }
                    _suspectLongPeriodUs = 0;
                    _haveSuspectLongPeriod = false;
                } else {
                    uint32_t currentUs = std::max<uint32_t>(1, _slaveTickPeriodUs);
                    uint32_t minimumSupportedPeriodUs = uint32_t(std::max(
                        1.f,
                        (60.f * 1000000.f * divisor) /
                            (SlaveMaxSupportedBpm * _ppqn)
                    ));

                    bool impossibleEarlyEdge = periodUs < minimumSupportedPeriodUs;
                    bool largeFastTransition =
                        _slaveTimingSamples >= 2 &&
                        uint64_t(periodUs) * 100 <
                            uint64_t(currentUs) * SlaveShortTransitionPercent;
                    bool largeSlowTransition =
                        _slaveTimingSamples >= 2 &&
                        uint64_t(periodUs) * 100 >
                            uint64_t(currentUs) * SlaveLongTransitionPercent;

                    if (impossibleEarlyEdge || largeFastTransition) {
                        _suspectShortPeriodUs = periodUs;
                        _haveSuspectShortPeriod = true;
                        pulseCredits = 0;
                    } else if (largeSlowTransition) {
                        _suspectLongPeriodUs = periodUs;
                        _haveSuspectLongPeriod = true;
                    } else {
                        measuredPeriodUs = periodUs;
                        haveMeasurement = true;

                        // Keep the raw pair around until after the timing
                        // estimator has consumed it. Swing detection should be
                        // compared with the newly refined mean, not the stale
                        // mean from before this edge; that is especially
                        // important for the very first long/short pair.
                        if (_havePreviousSlavePeriod) {
                            regularPairCandidate = true;
                            regularPairPreviousUs = _previousSlavePeriodUs;
                            regularPairCurrentUs = periodUs;
                            regularPairMeanBeforeUs = currentUs;
                            regularPairTimingSamples = _slaveTimingSamples;
                        }
                    }
                }
            }
        }
    }

    // Protect against clock overload. Short-edge candidates defer their tick
    // credit until they are either confirmed as real pulses or rejected as a
    // glitch. The bounded queue lets a newly discovered long/short pair settle
    // without dropping internal ticks; once learned, its intervals drive the
    // interpolation cadence directly.
    uint32_t credits = pulseCredits * divisor;
    _slaveSubTicksPending = std::min(
        _slaveSubTicksPending + credits,
        2 * divisor
    );

    if (haveMeasurement) {
        if (lockMeasurement) {
            resetSlaveSwingTracking();
            lockSlaveTiming(measuredPeriodUs, divisor);
        } else {
            updateSlaveTiming(measuredPeriodUs, divisor);
        }

        if (!lockMeasurement && !establishSwingPair && regularPairCandidate) {
            uint32_t pairAverageUs = uint32_t(
                (uint64_t(regularPairPreviousUs) + regularPairCurrentUs) / 2
            );
            uint32_t separationUs = regularPairPreviousUs > regularPairCurrentUs
                ? regularPairPreviousUs - regularPairCurrentUs
                : regularPairCurrentUs - regularPairPreviousUs;
            uint32_t meanUs = std::max<uint32_t>(1, _slaveTickPeriodUs);
            bool meaningfulSeparation =
                uint64_t(separationUs) * 100 >
                uint64_t(pairAverageUs) * SlaveSwingMinSeparationPercent;
            bool acquisitionPair = regularPairTimingSamples <= 1;
            bool straddlesEstablishedMean =
                (regularPairPreviousUs < regularPairMeanBeforeUs &&
                    regularPairCurrentUs > regularPairMeanBeforeUs) ||
                (regularPairPreviousUs > regularPairMeanBeforeUs &&
                    regularPairCurrentUs < regularPairMeanBeforeUs);

            if ((acquisitionPair || straddlesEstablishedMean) &&
                meaningfulSeparation &&
                periodsSimilar(pairAverageUs, meanUs, SlaveSwingPairTolerancePercent)) {
                establishSwingPair = true;
                swingPairAUs = regularPairPreviousUs;
                swingPairBUs = regularPairCurrentUs;
                // For an alternating pair the interval after the current edge
                // is expected to match the previous interval.
                swingNextExpectedUs = regularPairPreviousUs;
                nextInterpolationPeriodUs = swingNextExpectedUs;
            }
        }

        if (establishSwingPair) {
            setSlaveSwingPair(swingPairAUs, swingPairBUs, swingNextExpectedUs);
            if (preserveCurrentRawPeriod) {
                // The timing estimator consumed the pair mean above. Keep the
                // actual current edge as the previous raw interval so the next
                // tracked swing edge forms the correct long/short average.
                _previousSlavePeriodUs = swingPairBUs;
                _havePreviousSlavePeriod = true;
            }
        }

        if (nextInterpolationPeriodUs == 0) {
            nextInterpolationPeriodUs = _slaveTickPeriodUs;
        }
    }

    _lastSlaveTickUs = _elapsedUs;
    _haveLastSlaveTick = true;

    if (_slaveSubTickPeriodUs == 0) {
        _slaveSubTickPeriodUs = std::max<uint32_t>(1, _slaveTickPeriodUs / divisor);
    }
    if (nextInterpolationPeriodUs > 0) {
        _slaveSubTickPeriodUs = std::max<uint32_t>(1, nextInterpolationPeriodUs / divisor);
    }

    // Preserve the already scheduled interpolation phase while it is still
    // close to the external edge. Advancing `_nextSlaveSubTickUs` here would
    // skip one internal tick on every clean input pulse and accumulate phase
    // lag. Re-anchor only when the schedule is genuinely more than one
    // sub-tick behind. The next 100 us slave-timer ISR then emits the edge
    // tick with bounded latency. Signed modular subtraction remains valid
    // across uint32_t microsecond wrap.
    int32_t phaseErrorUs = static_cast<int32_t>(_elapsedUs - _nextSlaveSubTickUs);
    if (_nextSlaveSubTickUs == 0 || phaseErrorUs > static_cast<int32_t>(_slaveSubTickPeriodUs)) {
        _nextSlaveSubTickUs = _elapsedUs;
    }
}

void Clock::slaveStart(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state == State::MasterRunning || _mode == Mode::Master || (_state == State::SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(State::SlaveRunning);
    _activeSlave = slave;

    // Until enough external edges have arrived, use the configured internal
    // tempo as the sensible acquisition seed. Slave and master tempo remain
    // separate from this point onward.
    _slaveBpm = Clock::masterBpm();

    requestStart();
    resetTicks();

    _timer.disable();
    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveStop(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state != State::SlaveRunning || _mode == Mode::Master || _activeSlave != slave) {
        return;
    }

    setState(State::Idle);
    _activeSlave = -1;
    resetSlaveTiming();
    requestStop();

    _timer.disable();
}

void Clock::slaveContinue(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state != State::Idle || _mode == Mode::Master) {
        return;
    }

    setState(State::SlaveRunning);
    _activeSlave = slave;

    // Continue preserves the last detected slave BPM as acquisition seed.
    if (_slaveBpm <= 0.f) {
        _slaveBpm = Clock::masterBpm();
    }

    requestContinue();

    setupSlaveTimer();
    _timer.enable();
}

void Clock::slaveReset(int slave) {
    os::InterruptLock lock;

    if (!slaveEnabled(slave)) {
        return;
    }

    if (_state == State::MasterRunning || _mode == Mode::Master || (_state == State::SlaveRunning && _activeSlave != slave)) {
        return;
    }

    setState(State::Idle);
    _activeSlave = -1;
    resetSlaveTiming();
    requestReset();

    _timer.disable();
}

void Clock::slaveHandleMidi(int slave, uint8_t msg) {
    switch (MidiMessage::realTimeMessage(msg)) {
    case MidiMessage::Tick:
        slaveTick(slave);
        break;
    case MidiMessage::Start:
        slaveStart(slave);
        break;
    case MidiMessage::Stop:
        slaveStop(slave);
        break;
    case MidiMessage::Continue:
        slaveContinue(slave);
        break;
    default:
        break;
    }
}

void Clock::outputConfigure(int divisor, int pulse) {
    os::InterruptLock lock;
    _output.divisor = divisor;
    _output.pulse = pulse;
}

void Clock::outputConfigureSwing(int swing) {
    os::InterruptLock lock;
    _output.swing = swing;
}

#define CHECK(_event_)                  \
    if (_requestedEvents & _event_) {   \
        _requestedEvents &= ~_event_;   \
        return _event_;                 \
    }

Clock::Event Clock::checkEvent() {
    os::InterruptLock lock;

    if (_requestedEvents) {
        CHECK(Start)
        CHECK(Stop)
        CHECK(Continue)
        CHECK(Reset)
    }

    return Event(0);
}

#undef CHECK

bool Clock::checkTick(uint32_t *tick) {
    os::InterruptLock lock;

    if (_requestedEvents) {
        return false;
    }
    if (_tickProcessed < _tick) {
        *tick = _tickProcessed++;
        return true;
    }
    return false;
}

void Clock::onClockTimerTick() {
    os::InterruptLock lock;

    switch (_state) {
    case State::MasterRunning: {
        outputTick(_tick);
        ++_tick;
        _elapsedUs += _timer.period();
        break;
    }
    case State::SlaveRunning: {
        _elapsedUs += _timer.period();

        if (_slaveSubTicksPending > 0 && timeReached(_elapsedUs, _nextSlaveSubTickUs)) {
            outputTick(_tick);
            ++_tick;
            --_slaveSubTicksPending;
            _nextSlaveSubTickUs += _slaveSubTickPeriodUs;
        }

        if (_mode == Mode::Auto && _haveLastSlaveTick &&
            (_elapsedUs - _lastSlaveTickUs) > slaveTimeoutUs()) {
            slaveReset(_activeSlave);
        }
        break;
    }
    default:
        break;
    }
}

void Clock::resetTicks() {
    _tick = 0;
    _tickProcessed = 0;
    _slaveSubTicksPending = 0;
    _output.nextTick = 0;
}

void Clock::requestStart() {
    requestEvent(Start);
    outputMidiMessage(MidiMessage::Start);
    // outputMidiMessage(MidiMessage::Tick); // TODO: this seems wrong
    outputRun(true);
    outputReset(true);
}

void Clock::requestStop() {
    requestEvent(Stop);
    outputMidiMessage(MidiMessage::Stop);
    outputRun(false);
    outputReset(false);
}

void Clock::requestContinue() {
    requestEvent(Continue);
    outputMidiMessage(MidiMessage::Continue);
    outputRun(true);
    outputReset(false);
}

void Clock::requestReset() {
    requestEvent(Reset);
    outputMidiMessage(MidiMessage::Stop);
    outputRun(false);
    outputReset(true);
    outputClock(false);
}

void Clock::requestEvent(Event event) {
    _requestedEvents |= event;
}

void Clock::setState(State state) {
    _state = state;
}

void Clock::setupMasterTimer() {
    _elapsedUs = 0;
    uint32_t us = (60 * 1000000) / (_masterBpm * _ppqn);
    _timer.setPeriod(us);
}

void Clock::setupSlaveTimer() {
    _elapsedUs = 0;
    resetSlaveTiming();
    _timer.setPeriod(SlaveTimerPeriod);
}

void Clock::resetSlaveTiming() {
    _lastSlaveTickUs = 0;
    _haveLastSlaveTick = false;
    _previousSlavePeriodUs = 0;
    _havePreviousSlavePeriod = false;
    _slaveTimingSamples = 0;
    _suspectShortPeriodUs = 0;
    _haveSuspectShortPeriod = false;
    _suspectLongPeriodUs = 0;
    _haveSuspectLongPeriod = false;
    resetSlaveSwingTracking();
    _slaveTickPeriodUs = 0;
    _slaveSubTickPeriodUs = 0;
    _nextSlaveSubTickUs = 0;
    _slaveSubTicksPending = 0;
}

void Clock::resetSlaveSwingTracking() {
    _slaveSwingTracking = false;
    _slaveSwingShortPeriodUs = 0;
    _slaveSwingLongPeriodUs = 0;
    _slaveExpectedSwingPeriodUs = 0;
}

void Clock::setSlaveSwingPair(uint32_t aUs, uint32_t bUs, uint32_t nextExpectedUs) {
    _slaveSwingShortPeriodUs = std::min(aUs, bUs);
    _slaveSwingLongPeriodUs = std::max(aUs, bUs);
    _slaveExpectedSwingPeriodUs = std::max<uint32_t>(1, nextExpectedUs);
    _slaveSwingTracking = _slaveSwingLongPeriodUs > _slaveSwingShortPeriodUs;
}

void Clock::updateSlaveTiming(uint32_t periodUs, uint32_t divisor) {
    bool havePair = _havePreviousSlavePeriod;
    uint32_t candidateUs = periodUs;

    // A rolling two-interval average cancels the common long/short alternation
    // of a swung external clock while still producing a new estimate on every
    // pulse. The first measured interval remains immediately usable.
    if (havePair) {
        candidateUs = uint32_t((uint64_t(_previousSlavePeriodUs) + periodUs) / 2);
    }

    _previousSlavePeriodUs = periodUs;
    _havePreviousSlavePeriod = true;

    if (_slaveTimingSamples == 0 || (_slaveTimingSamples == 1 && havePair)) {
        // Acquisition: lock directly to the first raw measurement and then to
        // the first swing-compensated pair. This removes the old multi-second
        // startup lag while keeping swung clocks stable from the third edge.
        _slaveTickPeriodUs = candidateUs;
    } else {
        uint32_t currentUs = std::max<uint32_t>(1, _slaveTickPeriodUs);
        uint32_t deltaUs = candidateUs > currentUs ? candidateUs - currentUs : currentUs - candidateUs;

        // Use a real-time filter constant rather than a fixed number of pulse
        // samples. The old 16-sample window became dramatically slower as the
        // external PPQN was reduced. Large tempo moves use a much shorter time
        // constant; small deviations are treated as jitter.
        bool tempoMove = uint64_t(deltaUs) * 100 > uint64_t(currentUs) * 8;
        uint32_t filterTimeUs = tempoMove ? SlaveChangeFilterTimeUs : SlaveStableFilterTimeUs;

        uint64_t adjustment =
            (uint64_t(deltaUs) * periodUs) /
            (uint64_t(filterTimeUs) + periodUs);

        if (adjustment == 0 && deltaUs > 0) {
            adjustment = 1;
        }

        if (candidateUs > currentUs) {
            _slaveTickPeriodUs = currentUs + uint32_t(adjustment);
        } else {
            _slaveTickPeriodUs = currentUs - uint32_t(adjustment);
        }
    }

    ++_slaveTimingSamples;

    if (_slaveTickPeriodUs > 0) {
        _slaveBpm =
            (60.f * 1000000.f * divisor) /
            (float(_slaveTickPeriodUs) * _ppqn);
    }
}

void Clock::lockSlaveTiming(uint32_t periodUs, uint32_t divisor) {
    _slaveTickPeriodUs = std::max<uint32_t>(1, periodUs);
    _previousSlavePeriodUs = _slaveTickPeriodUs;
    _havePreviousSlavePeriod = true;
    _slaveTimingSamples = std::max<uint32_t>(2, _slaveTimingSamples + 1);
    _slaveBpm =
        (60.f * 1000000.f * divisor) /
        (float(_slaveTickPeriodUs) * _ppqn);
}

uint32_t Clock::slaveSeedPeriodUs(uint32_t divisor) const {
    float seedBpm = _slaveBpm > 0.f ? _slaveBpm : _masterBpm;
    seedBpm = std::max(seedBpm, 0.001f);

    uint32_t periodUs = uint32_t(
        (60.f * 1000000.f * divisor) /
        (seedBpm * _ppqn)
    );
    return std::max<uint32_t>(1, periodUs);
}

uint32_t Clock::slaveTimeoutUs() const {
    // Before the second edge there is no measured period yet. A generous but
    // finite acquisition window lets low-PPQN / slow external clocks deliver
    // that first real measurement instead of timing out from the unrelated
    // master-tempo seed.
    if (_slaveTimingSamples == 0) {
        return SlaveAcquisitionTimeoutUs;
    }

    uint32_t periodUs = std::max<uint32_t>(1, _slaveTickPeriodUs);

    // Once locked, three expected pulse periods gives low-PPQN and slow clocks
    // enough room for occasional missing edges while preserving a 500 ms
    // minimum timeout for dense clock streams.
    uint64_t timeoutUs = std::max<uint64_t>(
        SlaveMinTimeoutUs,
        uint64_t(periodUs) * 3
    );

    // timeReached()/signed modular comparisons require horizons below 2^31 us.
    return uint32_t(std::min<uint64_t>(timeoutUs, 0x7fffffffu));
}

bool Clock::timeReached(uint32_t now, uint32_t target) {
    return static_cast<int32_t>(now - target) >= 0;
}

bool Clock::periodsSimilar(uint32_t a, uint32_t b, uint32_t tolerancePercent) {
    uint32_t high = std::max(a, b);
    uint32_t low = std::min(a, b);
    return uint64_t(high - low) * 100 <= uint64_t(high) * tolerancePercent;
}

void Clock::outputMidiMessage(uint8_t msg) {
    os::InterruptLock lock;
    if (_listener) {
        _listener->onClockMidi(msg);
    }
}

void Clock::outputTick(uint32_t tick) {
    outputReset(false);

    if (tick % (_ppqn / 24) == 0) {
        outputMidiMessage(MidiMessage::Tick);
    }

    // generate output clock with swing

    auto applySwing = [this] (uint32_t tick) {
        return _output.swing != 0 ? Groove::applySwing(tick, _output.swing) : tick;
    };

    if (tick == _output.nextTick) {
        uint32_t divisor = _output.divisor;
        uint32_t clockDuration = std::max(uint32_t(1), uint32_t(bpm() * _ppqn * _output.pulse / (60 * 1000)));

        _output.nextTickOn = applySwing(_output.nextTick);
        _output.nextTickOff = std::min(_output.nextTickOn + clockDuration, applySwing(_output.nextTick + divisor) - 1);

        _output.nextTick += divisor;
    }

    if (tick == _output.nextTickOn) {
        outputClock(true);
    }

    if (tick == _output.nextTickOff) {
        outputClock(false);
    }
}

void Clock::outputClock(bool clock) {
    os::InterruptLock lock;

    if (clock != _outputState.clock) {
        _outputState.clock = clock;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}

void Clock::outputReset(bool reset) {
    os::InterruptLock lock;

    if (reset != _outputState.reset) {
        _outputState.reset = reset;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}

void Clock::outputRun(bool run) {
    os::InterruptLock lock;

    if (run != _outputState.run) {
        _outputState.run = run;
        if (_listener) {
            _listener->onClockOutput(_outputState);
        }
    }
}
