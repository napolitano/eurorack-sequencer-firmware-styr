/**
 * @file Mat4.h
 * @brief Declares the Mat4 component used by the shared math support.
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
#include "Vec4.h"
#include "Mat3.h"

/**
 * @brief Stores the fields required to represent mat4.
 */
struct Mat4 {
    /**
     * @brief Constructs a Mat4 instance.
     */
    Mat4();
    /**
     * @brief Constructs a Mat4 instance.
     *
     * @param[in] a11 Matrix element at row 1, column 1.
     * @param[in] a12 Matrix element at row 1, column 2.
     * @param[in] a13 Matrix element at row 1, column 3.
     * @param[in] a14 Matrix element at row 1, column 4.
     * @param[in] a21 Matrix element at row 2, column 1.
     * @param[in] a22 Matrix element at row 2, column 2.
     * @param[in] a23 Matrix element at row 2, column 3.
     * @param[in] a24 Matrix element at row 2, column 4.
     * @param[in] a31 Matrix element at row 3, column 1.
     * @param[in] a32 Matrix element at row 3, column 2.
     * @param[in] a33 Matrix element at row 3, column 3.
     * @param[in] a34 Matrix element at row 3, column 4.
     * @param[in] a41 Matrix element at row 4, column 1.
     * @param[in] a42 Matrix element at row 4, column 2.
     * @param[in] a43 Matrix element at row 4, column 3.
     * @param[in] a44 Matrix element at row 4, column 4.
     */
    Mat4(float a11, float a12, float a13, float a14,
         float a21, float a22, float a23, float a24,
         float a31, float a32, float a33, float a34,
         float a41, float a42, float a43, float a44);

    /**
     * @brief Returns the det.
     *
     * @return Determinant of the matrix.
     */
    float det() const;

    /**
     * @brief Transposes note data by the requested interval.
     */
    void transpose();
    /**
     * @brief Returns the transposed.
     *
     * @return Transposed copy of the matrix.
     */
    Mat4 transposed() const;
    /**
     * @brief Returns or applies the inverse representation.
     */
    void invert();
    /**
     * @brief Returns the inverted.
     *
     * @return Inverse of the value or matrix.
     */
    Mat4 inverted() const;
    /**
     * @brief Returns the pseudo inverted.
     *
     * @return Pseudo-inverse of the matrix.
     */
    Mat4 pseudoInverted() const;

    /**
     * @brief Returns the upper3x3.
     *
     * @return Upper-left 3x3 matrix extracted from this 4x4 matrix.
     */
    Mat3 upper3x3() const;

    /**
     * @brief Returns translate.
     *
     * @param[in] v Value or vector operand supplied by the caller.
     *
     * @return Translated/mapped value.
     */
    static Mat4 translate(const Vec3 &v);
    /**
     * @brief Returns scale.
     *
     * @param[in] s String/scalar operand supplied by the caller.
     *
     * @return Current/selected scale.
     */
    static Mat4 scale(const Vec3 &s);
    /**
     * @brief Returns rot xyz.
     *
     * @param[in] rot Rotation value or matrix used by the transform operation.
     *
     * @return Result of rotXYZ().
     */
    static Mat4 rotXYZ(const Vec3 &rot);
    /**
     * @brief Returns rot yzx.
     *
     * @param[in] rot Rotation value or matrix used by the transform operation.
     *
     * @return Result of rotYZX().
     */
    static Mat4 rotYZX(const Vec3 &rot);
    /**
     * @brief Returns rot axis.
     *
     * @param[in] axis Axis vector around which the rotation is constructed.
     * @param[in] angle Angle in the unit used by the geometry/game subsystem.
     *
     * @return Result of rotAxis().
     */
    static Mat4 rotAxis(const Vec3 &axis, float angle);

    /**
     * @brief Returns ortho.
     *
     * @param[in] l Left clipping-plane coordinate.
     * @param[in] r Radius or red-channel/scalar component as defined by the called interface.
     * @param[in] b Right/second value used by the comparison or binary operation.
     * @param[in] t Top clipping-plane coordinate.
     * @param[in] nearZ Near clipping-plane distance.
     * @param[in] farZ Far clipping-plane distance.
     *
     * @return Result of ortho().
     */
    static Mat4 ortho(float l, float r, float b, float t, float nearZ, float farZ);
    /**
     * @brief Returns perspective.
     *
     * @param[in] fov Vertical field of view used to construct the perspective transform.
     * @param[in] ratio Ratio used by the calculation.
     * @param[in] nearZ Near clipping-plane distance.
     * @param[in] farZ Far clipping-plane distance.
     *
     * @return Result of perspective().
     */
    static Mat4 perspective(float fov, float ratio, float nearZ, float farZ);
    /**
     * @brief Returns look at.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] fwd Forward direction vector used to construct the view transform.
     * @param[in] up Up direction vector used to orient the view transform.
     *
     * @return Result of lookAt().
     */
    static Mat4 lookAt(const Vec3 &pos, const Vec3 &fwd, const Vec3 &up);

    /**
     * @brief Provides named 4x4 matrix elements and contiguous array access to the same storage.
     */
    union {
         struct {
             float a11, a12, a13, a14;
             float a21, a22, a23, a24;
             float a31, a32, a33, a34;
             float a41, a42, a43, a44;
         };
         float a[16];
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
Mat4 operator*(const Mat4 &a, const Mat4 &b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec4 operator*(const Mat4 &a, const Vec4 &b);
/**
 * @brief Multiplies the operands component-wise or applies the documented transform.
 *
 * @param[in] a First/left operand or interpolation start value.
 * @param[in] b Second/right operand or interpolation end value.
 *
 * @return Result of applying the operator to the supplied operand(s).
 */
Vec3 operator*(const Mat4 &a, const Vec3 &b);
