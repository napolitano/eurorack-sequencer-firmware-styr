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

struct Mat3 {
    Mat3();
    Mat3(float a11, float a12, float a13,
         float a21, float a22, float a23,
         float a31, float a32, float a33);

    float det() const;

    void invert();
    Mat3 inverted() const;
    void transpose();
    Mat3 transposed() const;

    static Mat3 scale(float scale);
    static Mat3 rotateX(float theta);
    static Mat3 rotateY(float theta);
    static Mat3 rotateZ(float theta);
    static Mat3 transform2D(const Vec2 &translate, float rotate, float scale);

    union {
         struct {
             float a11, a12, a13;
             float a21, a22, a23;
             float a31, a32, a33;
         };
         float a[9];
     };
};

Mat3 operator*(const Mat3 &a, const Mat3 &b);
Vec2 operator*(const Mat3 &a, const Vec2 &b);
