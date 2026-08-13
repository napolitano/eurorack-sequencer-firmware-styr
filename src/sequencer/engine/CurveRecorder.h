/**
 * @file CurveRecorder.h
 * @brief Declares the CurveRecorder component used by the sequencer engine.
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

#include "model/Curve.h"

#include <array>

#include <cstdint>

/**
 * @brief Associates sampled CV values with Curve steps during realtime recording.
 */
class CurveRecorder {
public:
    /**
     * @brief Resets the CurveRecorder to its initial runtime state.
     */
    void reset() {
        _writeIndex = 0;
        _writeCount = 0;
    }

    /**
     * @brief Writes data to the underlying destination.
     *
     * @param[in] relativeTick Tick offset relative to the current sequence or step origin.
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     * @param[in] value Value to write.
     *
     * @return `true` if write; otherwise `false`.
     */
    bool write(uint32_t relativeTick, uint32_t divisor, float value) {
        bool result = false;

        // record step at the beginning of a step
        // this allows capturing the cv value both at the end of the current step and the start of the next step
        if (relativeTick % divisor == 0) {
            // fill up buffer with current value
            while (_writeIndex <= RecordBufferLength) {
                _buffer[_writeIndex++] = value;
            }

            // record step if recording has been enabled for the duration of the step
            if (_writeCount == divisor) {
                result = true;
            }

            reset();
        }

        uint32_t writeTo = ((relativeTick % divisor) * RecordBufferLength) / divisor;

        while (writeTo >= _writeIndex) {
            _buffer[_writeIndex] = value;
            ++_writeIndex;
        }

        ++_writeCount;

        return result;
    }

    /**
     * @brief Stores the result of matching a recorded sample/event to sequence timing.
     */
    struct Match {
        /**
         * @brief Runtime value representing min.
         */
        float min; ///< Minimum value accepted by the object.
        /**
         * @brief Runtime value representing max.
         */
        float max; ///< Maximum value accepted by the object.
        Curve::Type type; ///< Selected type that determines how `CurveRecorder` interprets or renders its data.
    };

    /**
     * @brief Returns the match curve.
     *
     * @return Curve-shape identifier matched by the recorder.
     */
    Match matchCurve() const {
        // determine range
        float curveMin = 1.f;
        float curveMax = 0.f;
        for (auto &value : _buffer) {
            curveMin = std::min(curveMin, value);
            curveMax = std::max(curveMax, value);
        }

        // compute squared error between a curve type and the recorded data
        auto curveError = [this, curveMin, curveMax] (Curve::Type type) {
            float error = 0.f;
            float x = 0.f;
            for (uint32_t i = 0; i < RecordBufferLength; ++i) {
                float yn = (_buffer[i] - curveMin) / (curveMax - curveMin);
                float y = Curve::eval(type, x);
                error += (yn - y) * (yn - y);
                x += (1.f / RecordBufferLength);
            }
            return error;
        };

        // determine best curve shape (least error)
        Curve::Type curveType = Curve::Type(0);
        float minError = curveError(curveType);
        for (Curve::Type type = Curve::Type(1); type < Curve::Last; type = Curve::Type(type + 1)) {
            float error = curveError(type);
            if (error < minError) {
                minError = error;
                curveType = type;
            }
        }

        return { curveMin, curveMax, curveType };
    }

private:
    /**
     * @brief Record buffer length constant used by this component.
     */
    static constexpr uint32_t RecordBufferLength = 16; ///< Number of recent CV samples retained by the curve recorder.

    /**
     * @brief Backing buffer used to stage data for the surrounding operation.
     */
    std::array<float, RecordBufferLength + 1> _buffer; ///< Backing buffer used to stage data for the surrounding operation.
    /**
     * @brief Zero-based write index; a negative/sentinel value represents no selection where applicable.
     */
    uint32_t _writeIndex; ///< Zero-based write index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Number of write items currently tracked or supported.
     */
    uint32_t _writeCount; ///< Number of write items currently tracked or supported.
};
