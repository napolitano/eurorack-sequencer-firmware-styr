/**
 * @file TapTempo.h
 * @brief Declares the TapTempo component used by the sequencer engine.
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

#include "core/utils/MovingAverage.h"
#include "drivers/HighResolutionTimer.h"

/**
 * @brief Estimates tempo from consecutive user taps while rejecting large discontinuities between tap intervals.
 */
class TapTempo {
public:
    /**
     * @brief Constructs a TapTempo instance.
     */
    TapTempo() {
        reset();
    }

    /**
     * @brief Resets the TapTempo to its initial runtime state.
     */
    void reset() {
        _lastTime = 0;
        _lastInterval = 0;
    }

    /**
     * @brief Returns tap.
     *
     * @param[in] bpm Tempo in beats per minute.
     *
     * @return Tempo derived from the accumulated tap intervals, in beats per minute.
     */
    float tap(float bpm) {
        uint32_t currentTime = HighResolutionTimer::us();
        uint32_t interval = currentTime - _lastTime;

        // reset averaging on first tap or if current interval is far off the last interval
        if (_lastTime == 0 || (interval < _lastInterval / 2 || interval > _lastInterval * 2)) {
            _intervalAverage.reset();
            _intervalAverage.push(uint32_t(60000000 / bpm));
        } else {
            _intervalAverage.push(interval);
            bpm = 60000000.f / _intervalAverage();
        }

        _lastTime = currentTime;
        _lastInterval = interval;

        return bpm;
    }

private:
    /**
     * @brief Most recently observed time.
     */
    uint32_t _lastTime; ///< Microsecond timestamp of the previous tap; zero marks the initial/no-tap state.
    /**
     * @brief Most recently observed interval.
     */
    uint32_t _lastInterval; ///< Interval between the two most recent taps, in microseconds.
    MovingAverage<uint32_t, 8> _intervalAverage; ///< Eight-sample moving average of accepted tap intervals, in microseconds.
};
