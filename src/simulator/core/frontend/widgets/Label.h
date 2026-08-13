/**
 * @file Label.h
 * @brief Declares the Label component used by the simulator frontend.
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

#include <string>
#include <memory>

namespace sim {

/**
 * @brief Provides label behavior for the desktop simulator.
 */
class Label : public Widget {
public:
    typedef std::shared_ptr<Label> Ptr;

    /**
     * @brief Constructs a Label instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     *
     * @note Includes an unnamed `const Color &color = Color(1.f, 1.f)` input parameter as declared by the inherited/interface signature.
     */
    Label(const Vector2f &pos, const Vector2f &size, const std::string &text, const Color &color = Color(1.f, 1.f)) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the text.
         */
        _text(text),
        /**
         * @brief Returns the color.
         */
        _color(color)
    {
    }

    /**
     * @brief Returns the text.
     *
     * @return Reference to the text.
     */
    const std::string &text() const { return _text; }
    /**
     * @brief Sets the text.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     */
    void setText(const std::string &text) { _text = text; }

    /**
     * @brief Returns the color.
     *
     * @return Reference to the color.
     */
    const Color &color() const { return _color; }
    /**
     * @brief Sets the color.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setColor(const Color &color) { _color = color; }

    /**
     * @brief Updates the Label for the current service cycle.
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
        renderer.drawText(_pos + _size * 0.5f, _text);
    }

private:
    std::string _text; ///< Text rendered by this label widget.
    /**
     * @brief Current drawing color/index.
     */
    Color _color; ///< Current drawing color/index.
};

} // namespace sim
