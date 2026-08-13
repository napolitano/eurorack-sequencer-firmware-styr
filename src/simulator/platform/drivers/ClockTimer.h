/**
 * @file ClockTimer.h
 * @brief Declares the ClockTimer component used by the simulator driver.
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

#include <cstdint>

/**
 * @brief Provides the clock timer hardware/platform abstraction.
 */
class ClockTimer {
public:
    /**
     * @brief Defines callbacks emitted by the surrounding component.
     */
    struct Listener {
        /**
         * @brief Handles the clock timer tick callback.
         */
        virtual void onClockTimerTick() = 0;
    };

    /**
     * @brief Constructs a ClockTimer instance.
     */
    ClockTimer() :
        /**
         * @brief Returns the simulator.
         */
        _simulator(sim::Simulator::instance()),
        _updateCallbackId(_simulator.addUpdateCallback([this] () { update(); }))
    /**
     * @brief Destroys the ClockTimer instance.
     */
    {}

    /**
     * @brief Destroys the ClockTimer instance.
     */
    ~ClockTimer() {
        _simulator.removeUpdateCallback(_updateCallbackId);
    }

    /**
     * @brief Initializes the ClockTimer and its runtime resources.
     */
    void init() {
    }

    /**
     * @brief Resets the ClockTimer to its initial runtime state.
     */
    void reset() {
        disable();
    }

    /**
     * @brief Enables the ClockTimer or feature.
     */
    void enable() {
        _enabled = true;
        _lastTicks = _simulator.ticks();
    }

    /**
     * @brief Disables the ClockTimer or feature.
     */
    void disable() {
        _enabled = false;
    }

    /**
     * @brief Returns the period.
     *
     * @return Configured timer period in microseconds.
     */
    uint32_t period() const {
        return _period;
    }

    /**
     * @brief Sets the period.
     *
     * @param[in] us Us, in microseconds.
     */
    void setPeriod(uint32_t us) {
        _period = us;
        _periodTicks = us * 0.001;
    }

    /**
     * @brief Sets the listener.
     *
     * @param[in] listener Listener that receives callbacks; `nullptr` detaches the listener where supported.
     */
    void setListener(Listener *listener) {
        _listener = listener;
    }

private:
    /**
     * @brief Updates the ClockTimer for the current service cycle.
     */
    void update() {
        if (!_enabled) {
            return;
        }

        // Simulator::step() invokes update callbacks at the beginning of the
        // 1 ms step and increments its public tick counter at the end. Account
        // for the interval that is currently being simulated; otherwise the
        // first wait after enable/re-enable is one millisecond short.
        double ticks = _simulator.ticks() + 1.0;
        while (ticks - _lastTicks >= _periodTicks) {
            _lastTicks += _periodTicks;
            if (_listener) {
                _listener->onClockTimerTick();
            }
        }
    }

    /**
     * @brief Reference to simulator owned by another component.
     */
    sim::Simulator &_simulator; ///< Reference to simulator owned by another component.
    sim::Simulator::UpdateCallbackId _updateCallbackId; ///< Simulator callback identifier used to unregister this timer update hook.
    /**
     * @brief Driver value representing period.
     */
    uint32_t _period = 0; ///< Configured timer period in microseconds.
    /**
     * @brief Driver value representing period ticks.
     */
    double _periodTicks = 0.0; ///< Configured timer period expressed in simulator service ticks.
    /**
     * @brief Listener that receives callbacks from this component.
     */
    Listener *_listener = nullptr; ///< Listener that receives callbacks from this component.
    /**
     * @brief Whether  is enabled.
     */
    bool _enabled = false; ///< Whether this option/source is enabled.
    /**
     * @brief Most recently observed ticks.
     */
    double _lastTicks; ///< Host-time timestamp of the previous timer update.
};
