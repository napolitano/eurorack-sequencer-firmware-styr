/**
 * @file Panel.h
 * @brief Declares the Panel component used by the simulator frontend.
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
 * @brief Provides panel behavior for the desktop simulator.
 */
class Panel : public Widget {
public:
    typedef std::shared_ptr<Panel> Ptr;

    /**
     * @brief Constructs a Panel instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    Panel(const Vector2f &pos, const Vector2f &size, const Color &color) :
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
     * @brief Updates the Panel for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Renders this component into the supplied target.
     *
     * @param[in] renderer Renderer used to create or update frontend resources.
     */
    virtual void render(Renderer &renderer) override {
        renderer.setColor(_color);
        renderer.fillRect(_pos, _size);
    }

private:
    /**
     * @brief Current drawing color/index.
     */
    Color _color; ///< Current drawing color/index.
};

} // namespace sim
