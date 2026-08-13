/**
 * @file Mat3.h
 * @brief Declares the Mat3 component used by the shared math support.
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

#include "Vec2.h"

/**
 * @brief Stores the fields required to represent mat3.
 */
struct Mat3 {
    /**
     * @brief Constructs a Mat3 instance.
     */
    Mat3();
    /**
     * @brief Constructs a Mat3 instance.
     *
     * @param[in] a11 Matrix element at row 1, column 1.
     * @param[in] a12 Matrix element at row 1, column 2.
     * @param[in] a13 Matrix element at row 1, column 3.
     * @param[in] a21 Matrix element at row 2, column 1.
     * @param[in] a22 Matrix element at row 2, column 2.
     * @param[in] a23 Matrix element at row 2, column 3.
     * @param[in] a31 Matrix element at row 3, column 1.
     * @param[in] a32 Matrix element at row 3, column 2.
     * @param[in] a33 Matrix element at row 3, column 3.
     */
    Mat3(float a11, float a12, float a13,
         float a21, float a22, float a23,
         float a31, float a32, float a33);

    /**
     * @brief Returns the det.
     *
     * @return Determinant of the matrix.
     */
    float det() const;

    /**
     * @brief Returns or applies the inverse representation.
     */
    void invert();
    /**
     * @brief Returns the inverted.
     *
     * @return Inverse of the value or matrix.
     */
    Mat3 inverted() const;
    /**
     * @brief Transposes note data by the requested interval.
     */
    void transpose();
    /**
     * @brief Returns the transposed.
     *
     * @return Transposed copy of the matrix.
     */
    Mat3 transposed() const;

    /**
     * @brief Returns scale.
     *
     * @param[in] scale Scale definition used for quantization or display.
     *
     * @return Current/selected scale.
     */
    static Mat3 scale(float scale);
    /**
     * @brief Returns rotate x.
     *
     * @param[in] theta Angular value used by the geometry/math operation.
     *
     * @return Result of rotateX().
     */
    static Mat3 rotateX(float theta);
    /**
     * @brief Returns rotate y.
     *
     * @param[in] theta Angular value used by the geometry/math operation.
     *
     * @return Result of rotateY().
     */
    static Mat3 rotateY(float theta);
    /**
     * @brief Returns rotate z.
     *
     * @param[in] theta Angular value used by the geometry/math operation.
     *
     * @return Result of rotateZ().
     */
    static Mat3 rotateZ(float theta);
    /**
     * @brief Returns transform2 d.
     *
     * @param[in] translate Translation vector applied by the affine transform.
     * @param[in] rotate Rotation angle applied by the affine transform.
     * @param[in] scale Scale definition used for quantization or display.
     *
     * @return Result of transform2D().
     */
    static Mat3 transform2D(const Vec2 &translate, float rotate, float scale);

    /**
     * @brief Provides named 3x3 matrix elements and contiguous array access to the same storage.
     */
    union {
         struct {
             float a11, a12, a13;
             float a21, a22, a23;
             float a31, a32, a33;
         };
         float a[9];
     };
};

/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Mat3 operator*(const Mat3 &a, const Mat3 &b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec2 operator*(const Mat3 &a, const Vec2 &b);
