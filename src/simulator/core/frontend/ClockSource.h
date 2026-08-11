/**
 * @file ClockSource.h
 * @brief Declares the ClockSource component used by the simulator frontend.
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

#include "core/Simulator.h"

#include <algorithm>
#include <iostream>

namespace sim {

class ClockSource {
public:
    ClockSource(Simulator &simulator, std::function<void()> handler) :
        _simulator(simulator),
        _handler(handler)
    {
        _simulator.addUpdateCallback([this] () { update(); });
    }

    void toggle() {
        _active = !_active;
        if (_active) {
            _lastTicks = _simulator.ticks();
            _nextTick = _lastTicks;
            _pulseIndex = 0;
        }
    }
    bool active() const {
        return _active;
    }

    int ppqn() const {
        return _ppqn;
    }

    void setPpqn(int ppqn) {
        _ppqn = std::max(1, ppqn);
    }

    double bpm() const {
        return _bpm;
    }

    void setBpm(double bpm) {
        _bpm = std::max(1.0, bpm);
    }

    int swing() const { return _swing; }

    void setSwing(int swing) {
        int newSwing = std::max(0, std::min(100, swing));
        if (newSwing != _swing) {
            _swing = newSwing;
            std::cout << "[sim] ClockSource: swing set to " << _swing << std::endl;
        }
    }

    void update() {
        if (_active) {
            double currentTicks = _simulator.ticks();
            double baseInterval = clockInterval() * 1000.0; // ms per sub-beat

            // Map swing [0..100] -> k in [-maxDelta .. +maxDelta], where k==0 => no swing
            // We cap maxDelta to 0.5 so intervals remain positive and avoid zero-length pulses.
            const double maxDelta = 0.5;
            double k = ((_swing - 50.0) / 50.0) * maxDelta;

            // First and second intervals keep average == baseInterval
            double firstInterval = baseInterval * (1.0 - k);
            double secondInterval = baseInterval * (1.0 + k);

            // First fire immediately when enabling
            if (_nextTick <= 0.0) {
                _nextTick = _lastTicks;
                std::cout << "[sim] ClockSource: start, swing=" << _swing << " firstInterval=" << firstInterval << " secondInterval=" << secondInterval << std::endl;
            }

            while (_nextTick <= currentTicks) {
                if (_handler) {
                    _handler();
                }

                if ((_pulseIndex & 1) == 0) {
                    _nextTick += firstInterval;
                } else {
                    _nextTick += secondInterval;
                }
                ++_pulseIndex;
            }

            // keep lastTicks updated for compatibility
            _lastTicks = currentTicks;
        }
    }

private:
    double clockInterval() {
        return 60.0 / (_bpm * _ppqn);
    }

    Simulator &_simulator;
    std::function<void()> _handler;

    bool _active = false;
    // Default PPQN must match engine expected external pulse rate.
    // Formula: CONFIG_SEQUENCE_PPQN (48) / clockInputDivisor (default 12) = 4
    // clockInputDivisor=12 (1/16) = 4 ext pulses per QN
    // clockInputDivisor=24 (1/8)  = 2 ext pulses per QN
    // clockInputDivisor=6  (1/32) = 8 ext pulses per QN
    int _ppqn = 4;
    double _bpm = 120.0;

    int _swing = 50; // 0..100, 50 == no swing

    double _lastTicks = 0.0;
    double _nextTick = 0.0;
    int _pulseIndex = 0;
};

} // namespace sim
