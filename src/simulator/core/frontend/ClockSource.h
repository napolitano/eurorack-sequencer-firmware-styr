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

/**
 * @brief Provides clock source behavior for the desktop simulator.
 */
class ClockSource {
public:
    /**
     * @brief Constructs a ClockSource instance.
     *
     * @param[in] simulator Simulator instance associated with the frontend/platform object.
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    ClockSource(Simulator &simulator, std::function<void()> handler) :
        /**
         * @brief Returns the simulator.
         */
        _simulator(simulator),
        /**
         * @brief Returns the handler.
         */
        _handler(handler),
        _updateCallbackId(_simulator.addUpdateCallback([this] () { update(); }))
    /**
     * @brief Destroys the ClockSource instance.
     */
    {}

    /**
     * @brief Destroys the ClockSource instance.
     */
    ~ClockSource() {
        _simulator.removeUpdateCallback(_updateCallbackId);
    }

    /**
     * @brief Toggles the current state.
     */
    void toggle() {
        _active = !_active;
        if (_active) {
            _lastTicks = _simulator.ticks();
            _nextTick = _lastTicks;
            _pulseIndex = 0;
        }
    }
    /**
     * @brief Returns the active.
     *
     * @return `true` if active; otherwise `false`.
     */
    bool active() const {
        return _active;
    }

    /**
     * @brief Returns the ppqn.
     *
     * @return Sequencer clock resolution in pulses per quarter note (PPQN).
     */
    int ppqn() const {
        return _ppqn;
    }

    /**
     * @brief Sets the ppqn.
     *
     * @param[in] ppqn New clock resolution in pulses per quarter note to store or apply.
     */
    void setPpqn(int ppqn) {
        _ppqn = std::max(1, ppqn);
    }

    /**
     * @brief Returns the bpm.
     *
     * @return Tempo in beats per minute.
     */
    double bpm() const {
        return _bpm;
    }

    /**
     * @brief Sets the bpm.
     *
     * @param[in] bpm Tempo in beats per minute.
     */
    void setBpm(double bpm) {
        _bpm = std::max(1.0, bpm);
    }

    /**
     * @brief Returns the swing.
     *
     * @return Configured swing amount in the model-defined percentage/range.
     */
    int swing() const { return _swing; }

    /**
     * @brief Sets the swing.
     *
     * @param[in] swing Swing setting in the model-defined range.
     */
    void setSwing(int swing) {
        int newSwing = std::max(0, std::min(100, swing));
        if (newSwing != _swing) {
            _swing = newSwing;
            std::cout << "[sim] ClockSource: swing set to " << _swing << std::endl;
        }
    }

    /**
     * @brief Updates the ClockSource for the current service cycle.
     */
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
    /**
     * @brief Returns the clock interval.
     *
     * @return Current host clock interval used by the clock source.
     */
    double clockInterval() {
        return 60.0 / (_bpm * _ppqn);
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    Simulator &_simulator; ///< Reference to simulator owned by another component.
    /**
     * @brief Returns the stored callable or opaque platform value.
     *
     * @return The void value.
     */
    std::function<void()> _handler; ///< Callback invoked for each generated clock pulse.
    Simulator::UpdateCallbackId _updateCallbackId; ///< Simulator callback identifier used to unregister the clock-source update hook.

    /**
     * @brief Whether  is currently active.
     */
    bool _active = false; ///< Whether this object is currently active.
    // Default PPQN must match engine expected external pulse rate.
    // Formula: CONFIG_SEQUENCE_PPQN (48) / clockInputDivisor (default 12) = 4
    // clockInputDivisor=12 (1/16) = 4 ext pulses per QN
    // clockInputDivisor=24 (1/8)  = 2 ext pulses per QN
    // clockInputDivisor=6  (1/32) = 8 ext pulses per QN
    int _ppqn = 4; ///< Sequencer clock resolution in pulses per quarter note (PPQN).
    /**
     * @brief Bpm, in beats per minute.
     */
    double _bpm = 120.0; ///< Bpm, in beats per minute.

    /**
     * @brief Simulator value representing swing.
     */
    int _swing = 50; ///< Clock swing amount in percent, where 50 represents straight timing.

    /**
     * @brief Most recently observed ticks.
     */
    double _lastTicks = 0.0; ///< Host-time timestamp of the previous clock-source update.
    /**
     * @brief Next tick scheduled or expected by this component.
     */
    double _nextTick = 0.0; ///< Next tick scheduled or expected by this component.
    /**
     * @brief Zero-based pulse index; a negative/sentinel value represents no selection where applicable.
     */
    int _pulseIndex = 0; ///< Zero-based pulse index; a negative/sentinel value represents no selection where applicable.
};

} // namespace sim
