/**
 * @file MovingAverage.h
 * @brief Declares the MovingAverage component used by the shared core support.
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

#include <algorithm>

/**
 * @brief Maintains a fixed-window arithmetic moving average with constant-time updates.
 */
template<typename T, size_t History>
/**
 * @brief Maintains a fixed-window arithmetic moving average over the most recent samples.
 */
class MovingAverage {
public:
    /**
     * @brief Constructs a MovingAverage instance.
     */
    MovingAverage() {
        reset();
    }

    /**
     * @brief Resets the MovingAverage to its initial runtime state.
     */
    void reset() {
        for (size_t i = 0; i < History; ++i) {
            _history[i] = T(0);
        }
        _sum = T(0);
        _index = 0;
        _count = 0;
    }

    /**
     * @brief Appends the supplied sample, item, or event.
     *
     * @param[in] sample Audio/CV/data sample consumed by the operation.
     */
    void push(T sample) {
        _sum -= _history[_index];
        _sum += sample;
        _history[_index] = sample;
        _index = (_index + 1) % History;
        _count = std::min(_count + 1, History);
    }

    /**
     * @brief Returns the arithmetic mean of the currently retained samples.
     *
     * @return Average of the retained samples; returns `T(0)` before the first sample is pushed.
     */
    T operator()() const {
        return _sum / (_count == 0 ? 1 : _count);
    }

private:
    T _history[History]; ///< Owned fixed-size collection of history.
    /**
     * @brief Returns the t.
     *
     * @return The t value.
     */
    T _sum = T(0); ///< Sum of the currently retained samples, maintained incrementally for O(1) averaging. ///< Sum of the currently retained samples, maintained incrementally for O(1) averaging.
    /**
     * @brief Zero-based index; a negative/sentinel value represents no selection where applicable.
     */
    size_t _index = 0; ///< Ring-buffer slot that will be replaced by the next sample.
    size_t _count = 0; ///< Number of valid samples currently contributing to the average, capped at `History`.
};
