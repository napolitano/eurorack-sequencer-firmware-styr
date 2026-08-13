/**
 * @file FrameBuffer.h
 * @brief Declares the FrameBuffer component used by the shared graphics support.
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

#include <cstdint>

/**
 * @brief Stores packed display pixels in a fixed-size framebuffer.
 */
template<typename T>
/**
 * @brief Owns the pixel storage and dimensions of a fixed-format framebuffer.
 */
class FrameBuffer {
public:
    /**
     * @brief Constructs a FrameBuffer instance.
     *
     * @param[in] width Width in pixels or the coordinate domain used by the caller.
     * @param[in] height Height in pixels or the coordinate domain used by the caller.
     * @param[in] buffer Buffer that supplies or receives the bytes/elements processed by the operation.
     */
    FrameBuffer(int width, int height, T *buffer) :
        /**
         * @brief Returns the width.
         */
        _width(width),
        /**
         * @brief Returns the height.
         */
        _height(height),
        /**
         * @brief Performs the size operation for this FrameBuffer.
         *
         * @param[in] height Height supplied to the operation.
         */
        _size(width * height),
        /**
         * @brief Returns the data.
         */
        _data(buffer)
    {}

    /**
     * @brief Returns the width.
     *
     * @return Width in pixels.
     */
    int width() const { return _width; }
    /**
     * @brief Returns the height.
     *
     * @return Height in pixels.
     */
    int height() const { return _height; }

    /**
     * @brief Returns the data.
     *
     * @return Pointer to the data; `nullptr` when no value is available.
     */
    const T *data() const { return _data; }
          /**
           * @brief Returns the data.
           *
           * @return Pointer to the data; `nullptr` when no value is available.
           */
          T *data()       { return _data; }

    /**
     * @brief Begins the operation or measurement interval.
     *
     * @return Pointer to the begin; `nullptr` when no value is available.
     */
    const T *begin() const { return _data; }
          /**
           * @brief Begins the operation or measurement interval.
           *
           * @return Pointer to the begin; `nullptr` when no value is available.
           */
          T *begin()       { return _data; }

    /**
     * @brief Ends the operation or measurement interval.
     *
     * @return Pointer to the end; `nullptr` when no value is available.
     */
    const T *end() const { return &_data[_size]; }
          /**
           * @brief Ends the operation or measurement interval.
           *
           * @return Pointer to the end; `nullptr` when no value is available.
           */
          T *end()       { return &_data[_size]; }

    /**
     * @brief Fills the target region or buffer using the current/supplied value.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void fill(const T value) {
        std::fill(begin(), end(), value);
    }

    /**
     * @brief Implements operator() for this type.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return Reference to the operator().
     */
    const T &operator()(int x, int y) const {
        return _data[y * _width + x];
    }

    /**
     * @brief Implements operator() for this type.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     *
     * @return Reference to the operator().
     */
    T &operator()(int x, int y) {
        return _data[y * _width + x];
    }

private:
    /**
     * @brief Width in pixels or the owning coordinate domain.
     */
    int _width; ///< Width in pixels or the owning coordinate domain.
    /**
     * @brief Height in pixels or the owning coordinate domain.
     */
    int _height; ///< Height in pixels or the owning coordinate domain.
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    int _size; ///< Size of the associated data in bytes/elements as defined by this type.
    /**
     * @brief Pointer to data; `nullptr` denotes that no object/resource is assigned.
     */
    T *_data; ///< Pointer to data; `nullptr` denotes that no object/resource is assigned.
};

typedef FrameBuffer<uint8_t> FrameBuffer8bit;
