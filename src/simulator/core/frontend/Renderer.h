/**
 * @file Renderer.h
 * @brief Declares the Renderer component used by the simulator frontend.
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

#include "Common.h"

#include <string>

struct NVGcontext;

namespace sim {

/**
 * @brief Provides renderer behavior for the desktop simulator.
 */
class Renderer {
public:
    /**
     * @brief Constructs a Renderer instance.
     *
     * @param[in] window SDL window that owns the OpenGL rendering context.
     */
    Renderer(SDL_Window *window);
    /**
     * @brief Destroys the Renderer instance.
     */
    ~Renderer();

    /**
     * @brief Returns the nvg.
     *
     * @return Pointer to the nvg; `nullptr` when no value is available.
     */
    NVGcontext *nvg() const { return _nvg; }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() ;
    /**
     * @brief Sets the color.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setColor(const Color &color);
    /**
     * @brief Draws a rectangle outline.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void drawRect(const Vector2f &pos, const Vector2f &size);
    /**
     * @brief Fills a rectangular region.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void fillRect(const Vector2f &pos, const Vector2f &size);
    /**
     * @brief Draws ellipse.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void drawEllipse(const Vector2f &pos, const Vector2f &size);
    /**
     * @brief Fills ellipse.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void fillEllipse(const Vector2f &pos, const Vector2f &size);
    /**
     * @brief Draws line.
     *
     * @param[in] p1 First endpoint of the rendered line/primitive in canvas coordinates.
     * @param[in] p2 Second endpoint of the rendered line/primitive in canvas coordinates.
     */
    void drawLine(const Vector2f &p1, const Vector2f &p2);
    /**
     * @brief Draws arc.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] rotation Rotation angle applied to the rendered primitive.
     */
    void drawArc(const Vector2f &pos, const Vector2f &size, float rotation);

    /**
     * @brief Draws a text string at the requested position.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     */
    void drawText(const Vector2f &pos, const std::string &text);

    /**
     * @brief Presents the current rendered frame.
     */
    void present();

private:
    /**
     * @brief Pointer to window; `nullptr` denotes that no object/resource is assigned.
     */
    SDL_Window *_window; ///< Pointer to window; `nullptr` denotes that no object/resource is assigned.
    SDL_GLContext _context; ///< OpenGL context associated with the renderer SDL window.
    /**
     * @brief Pointer to nvg; `nullptr` denotes that no object/resource is assigned.
     */
    NVGcontext *_nvg; ///< Pointer to nvg; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Simulator value representing font.
     */
    int _font; ///< NanoVG font handle used for text rendering; negative values indicate an unavailable font.
};

} // namespace sim
