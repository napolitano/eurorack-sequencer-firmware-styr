/**
 * @file Vec4.h
 * @brief Declares the Vec4 component used by the shared math support.
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

#include "Vec3.h"

#include <algorithm>

#include <cmath>

/**
 * @brief Stores the fields required to represent vec4.
 */
struct Vec4 {

    /**
     * @brief Constructs a Vec4 instance.
     */
    Vec4() {
        zero();
    }

    /**
     * @brief Constructs a Vec4 instance.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    Vec4(float c) {
        set(c);
    }

    /**
     * @brief Constructs a Vec4 instance.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] z Z coordinate or scalar z component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    Vec4(float x, float y, float z, float w) {
        set(x, y, z, w);
    }

    /**
     * @brief Constructs a Vec4 instance.
     *
     * @param[in] v Value or vector operand supplied by the caller.
     */
    Vec4(const Vec4 &v) {
        set(v.x, v.y, v.z, v.w);
    }

    /**
     * @brief Constructs a Vec4 instance.
     *
     * @param[in] v Value or vector operand supplied by the caller.
     */
    Vec4(const Vec3 &v) {
        set(v.x, v.y, v.z, 1.f);
    }

    /**
     * @brief Returns the additive zero value for this type.
     */
    void zero() {
        set(0.f);
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    void set(float c) {
        set(c, c, c, c);
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] z Z coordinate or scalar z component.
     * @param[in] w Width in pixels or the coordinate domain used by the caller.
     */
    void set(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    /**
     * @brief Returns or applies the inverse representation.
     */
    void invert() {
        *this = inverted();
    }

    /**
     * @brief Returns the inverted.
     *
     * @return Inverse of the value or matrix.
     */
    Vec4 inverted() const {
        return Vec4(1.f / x, 1.f / y, 1.f / z, 1.f / w);
    }

    /**
     * @brief Returns dot.
     *
     * @param[in] b Right/second value used by the comparison or binary operation.
     *
     * @return Result of dot().
     */
    float dot(const Vec4 &b) const {
        return x * b.x + y * b.y + z * b.z + w * b.w;
    }

    /**
     * @brief Returns the length.
     *
     * @return Number of length represented by this object.
     */
    float length() const {
        return std::sqrt(lengthSqr());
    }

    /**
     * @brief Returns the length sqr.
     *
     * @return Squared Euclidean length of the vector.
     */
    float lengthSqr() const {
        return x * x + y * y + z * z + w * w;
    }

    /**
     * @brief Normalizes the value to this type's canonical range.
     */
    void normalize() {
        *this = normalized();
    }

    /**
     * @brief Returns the normalized.
     *
     * @return Normalized copy with unit length where the input permits normalization.
     */
    Vec4 normalized() const {
        float invLength = 1.f / length();
        return Vec4(invLength * x, invLength * y, invLength * z, invLength * w);
    }

    /**
     * @brief Returns the abs.
     *
     * @return Component-wise absolute value.
     */
    Vec4 abs() const {
        return Vec4(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
    }

    /**
     * @brief Reports whether nan.
     *
     * @return `true` if nan; otherwise `false`.
     */
    bool isNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z) || std::isnan(w);
    }

    /**
     * @brief Returns min components.
     *
     * @param[in] v Value or vector operand supplied by the caller.
     *
     * @return Min components supported by this type.
     */
    Vec4 minComponents(const Vec4 &v) const {
        return Vec4(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z), std::min(w, v.w));
    }

    /**
     * @brief Returns max components.
     *
     * @param[in] v Value or vector operand supplied by the caller.
     *
     * @return Max components supported by this type.
     */
    Vec4 maxComponents(const Vec4 &v) const {
        return Vec4(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z), std::max(w, v.w));
    }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] index Zero-based component/element index.
     *
     * @return Reference/value of the component at the requested zero-based index.
     */
    float operator[](const int index) const {
        return a[index];
    }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] index Zero-based component/element index.
     *
     * @return Reference to the operator[].
     */
    float &operator[](const int index) {
        return a[index];
    }

    /**
     * @brief Adds another vector component-wise into this vector.
     *
     * @param[in] b Vector operand applied component-wise.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator+=(const Vec4 &b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;
        return *this;
    }

    /**
     * @brief Subtracts another vector component-wise into this vector.
     *
     * @param[in] b Vector operand applied component-wise.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator-=(const Vec4 &b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        w -= b.w;
        return *this;
    }

    /**
     * @brief Multiplies another vector component-wise into this vector.
     *
     * @param[in] b Vector operand applied component-wise.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator*=(const Vec4 &b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        w *= b.w;
        return *this;
    }

    /**
     * @brief Divides another vector component-wise into this vector.
     *
     * @param[in] b Vector operand applied component-wise.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator/=(const Vec4 &b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        w /= b.w;
        return *this;
    }

    /**
     * @brief Multiplies every vector component by a scalar in place.
     *
     * @param[in] b Scalar operand applied to every component.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator*=(float b) {
        x *= b;
        y *= b;
        z *= b;
        w *= b;
        return *this;
    }

    /**
     * @brief Divides every vector component by a scalar in place.
     *
     * @param[in] b Scalar operand applied to every component.
     *
     * @return Reference to this vector after the in-place operation.
     */
    Vec4 &operator/=(float b) {
        x /= b;
        y /= b;
        z /= b;
        w /= b;
        return *this;
    }

     /**
      * @brief Provides named vector components and indexed array access to the same storage.
      */
     union {
         struct { float x, y, z, w; };
         float a[4];
     };
};

/**
 * @brief Subtracts the operands or negates the operand.
 *
 * @param[in] a First/left operand or interpolation start value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator-(const Vec4 &a);
/**
 * @brief Adds the operands.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator+(const Vec4 &a, const Vec4 &b);
/**
 * @brief Subtracts the operands or negates the operand.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator-(const Vec4 &a, const Vec4 &b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator*(const Vec4 &a, const Vec4 &b);
/**
 * @brief Divides the operands component-wise.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator/(const Vec4 &a, const Vec4 &b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator*(const Vec4 &a, float b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator*(float a, const Vec4 &b);
/**
 * @brief Divides the operands component-wise.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator/(const Vec4 &a, float b);
/**
 * @brief Divides the operands component-wise.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator/(float a, const Vec4 &b);
/**
 * @brief Compares the operands for greater-than ordering.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
bool operator>(const Vec4 &a, const Vec4 &b);
/**
 * @brief Compares the operands for less-than ordering.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return `true` when the documented condition is satisfied; otherwise `false`.
 */
bool operator<(const Vec4 &a, const Vec4 &b);
bool operator>=(const Vec4 &a, const Vec4 &b);
bool operator<=(const Vec4 &a, const Vec4 &b);
bool operator==(const Vec4 &a, const Vec4 &b);
bool operator!=(const Vec4 &a, const Vec4 &b);
