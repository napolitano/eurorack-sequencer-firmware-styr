/**
 * @file Math.h
 * @brief Declares the Math component used by the shared math support.
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

#include <cmath>
#include <cstdint>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/**
 * @brief Clamps a value to the inclusive minimum/maximum range.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] min Inclusive minimum permitted result.
 * @param[in] max Inclusive maximum permitted result.
 *
 * @return Computed result in the domain described by this function.
 */
template<typename T>
static T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

/**
 * @brief Linearly interpolates between two values.
 *
 * @param[in] t Interpolation factor; 0 selects `a` and 1 selects `b`.
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Computed result in the domain described by this function.
 */
template<typename T, typename S>
static S lerp(T t, S a, S b) {
    return (T(1) - t) * a + t * b;
}

/**
 * @brief Returns the next power of two relative to the supplied integer.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] strict When `true`, excludes `value` itself when it is already a power of two.
 *
 * @return Computed result in the domain described by this function.
 */
uint32_t nextPowerOfTwo(uint32_t value, bool strict = true);
/**
 * @brief Returns the previous power of two relative to the supplied integer.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] strict When `true`, excludes `value` itself when it is already a power of two.
 *
 * @return Computed result in the domain described by this function.
 */
uint32_t prevPowerOfTwo(uint32_t value, bool strict = true);

/**
 * @brief Computes a modulo result whose sign is normalized to the divisor.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Computed result in the domain described by this function.
 */
static int modulo(int a, int b) {
    int m = a % b;
    if (m < 0) {
        m = (b < 0) ? m - b : m + b;
    }
    return m;
}

/**
 * @brief Divides two integers with rounding toward negative infinity.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Computed result in the domain described by this function.
 */
inline int roundDownDivide(int a, int b) {
    return a >= 0 ? (a / b) : (a - b + 1) / b;
}

/**
 * @brief Converts an angle from degrees to radians.
 *
 * @param[in] deg Angle in degrees.
 *
 * @return Computed result in the domain described by this function.
 */
inline float deg2rad(float deg) {
    return deg / 180.f * M_PI;
}

/**
 * @brief Converts an angle from radians to degrees.
 *
 * @param[in] rad Angle in radians.
 *
 * @return Computed result in the domain described by this function.
 */
inline float rad2deg(float rad) {
    return rad * 180.f / M_PI;
}

/**
 * @brief Reports whether an integer contains more than one set bit.
 *
 * @param[in] x Integer bit field to test.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
template<typename T>
bool multipleBitsSet(T x) {
    return x & (x - 1);
}

/**
 * @brief Reports whether an integer contains exactly one set bit.
 *
 * @param[in] x Integer bit field to test.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
template<typename T>
bool exactlyOneBitSet(T x) {
    return x && !multipleBitsSet(x);
}
