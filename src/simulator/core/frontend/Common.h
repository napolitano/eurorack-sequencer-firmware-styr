/**
 * @file Common.h
 * @brief Declares the Common component used by the simulator frontend.
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

#include "Vector.h"

#include <SDL.h>

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <functional>
#include <cstdint>
#include <cmath>

namespace sim {

static const float PI = 3.141592653589793f; ///< Single-precision value of pi used by simulator geometry and rendering helpers. ///< Single-precision value of pi used by simulator geometry and rendering helpers.
static const float TWO_PI = 6.283185307179586f; ///< Single-precision value of two pi used for full-turn angular calculations. ///< Single-precision value of two pi used for full-turn angular calculations.

/**
 * @brief Provides color behavior for the desktop simulator.
 */
class Color : public Vector4f {
public:
    /**
     * @brief Constructs a Color instance.
     */
    Color() : Vector4f(0.f, 0.f, 0.f, 0.f) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    explicit Color(const Vector4f &color) : Vector4f(color) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     * @param[in] alpha Alpha/opacity component used for rendering.
     */
    explicit Color(const Vector3f &color, float alpha) : Color(color[0], color[1], color[2], alpha) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     * @param[in] alpha Alpha/opacity component used for rendering.
     */
    explicit Color(const Vector3i &color, int alpha) : Color(Vector3f(color) / 255.f, alpha / 255.f) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    explicit Color(const Vector3f &color) : Color(color, 1.0f) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    explicit Color(const Vector3i &color) : Color((Vector3f(color) / 255.f)) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    explicit Color(const Vector4i &color) : Color((Vector4f(color) / 255.f)) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] intensity LED/color/intensity value in the range defined by the owning interface.
     * @param[in] alpha Alpha/opacity component used for rendering.
     */
    explicit Color(float intensity, float alpha) : Color(Vector3f(intensity), alpha) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] intensity LED/color/intensity value in the range defined by the owning interface.
     * @param[in] alpha Alpha/opacity component used for rendering.
     */
    explicit Color(int intensity, int alpha) : Color(Vector3i(intensity), alpha) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] r Radius or red-channel/scalar component as defined by the called interface.
     * @param[in] g Green/scalar component as defined by the called interface.
     * @param[in] b Right/second value used by the comparison or binary operation.
     * @param[in] a Left/first value used by the comparison or binary operation.
     */
    explicit Color(float r, float g, float b, float a) : Color(Vector4f(r, g, b, a)) {}
    /**
     * @brief Constructs a Color instance.
     *
     * @param[in] r Radius or red-channel/scalar component as defined by the called interface.
     * @param[in] g Green/scalar component as defined by the called interface.
     * @param[in] b Right/second value used by the comparison or binary operation.
     * @param[in] a Left/first value used by the comparison or binary operation.
     */
    explicit Color(int r, int g, int b, int a) : Color(Vector4i(r, g, b, a)) {}

    /**
     * @brief Returns the r.
     *
     * @return Reference to the r.
     */
    const float &r() const { return x(); }
          /**
           * @brief Returns the r.
           *
           * @return Reference to the r.
           */
          float &r()       { return x(); }

    /**
     * @brief Returns the g.
     *
     * @return Reference to the g.
     */
    const float &g() const { return y(); }
          /**
           * @brief Returns the g.
           *
           * @return Reference to the g.
           */
          float &g()       { return y(); }

    /**
     * @brief Returns the b.
     *
     * @return Reference to the b.
     */
    const float &b() const { return z(); }
          /**
           * @brief Returns the b.
           *
           * @return Reference to the b.
           */
          float &b()       { return z(); }

    /**
     * @brief Returns the a.
     *
     * @return Reference to the a.
     */
    const float &a() const { return w(); }
          /**
           * @brief Returns the a.
           *
           * @return Reference to the a.
           */
          float &a()       { return w(); }

    /**
     * @brief Returns the rgba.
     *
     * @return Packed RGBA color value.
     */
    uint32_t rgba() const {
        return (
            std::min(255, int(std::floor(x() * 255))) |
            std::min(255, int(std::floor(y() * 255))) << 8 |
            std::min(255, int(std::floor(z() * 255))) << 16 |
            std::min(255, int(std::floor(w() * 255))) << 24
        );
    }
};

} // namespace sim
