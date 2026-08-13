/**
 * @file Led.h
 * @brief Declares the Led component used by the simulator frontend.
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

#include "../Widget.h"

namespace sim {

/**
 * @brief Provides led behavior for the desktop simulator.
 */
class Led : public Widget {
public:
    typedef std::shared_ptr<Led> Ptr;

    /**
     * @brief Constructs a Led instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     *
     * @note Includes an unnamed `const Color &color = Color(0.f, 1.f)` input parameter as declared by the inherited/interface signature.
     */
    Led(const Vector2f &pos, const Vector2f &size, const Color &color = Color(0.f, 1.f)) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the color.
         */
        _color(color)
    {}

    /**
     * @brief Returns the color.
     *
     * @return Reference to the color.
     */
    const Color &color() const { return _color; }
          /**
           * @brief Returns the color.
           *
           * @return Reference to the color.
           */
          Color &color()       { return _color; }

    /**
     * @brief Updates the Led for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Renders this component into the supplied target.
     *
     * @param[in] renderer Renderer used to create or update frontend resources.
     */
    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawEllipse(_pos, _size);
        renderer.setColor(_color);
        renderer.fillEllipse(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f));
    }

private:
    /**
     * @brief Current drawing color/index.
     */
    Color _color; ///< Current drawing color/index.
};

} // namespace sim
