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

class Label : public Widget {
public:
    typedef std::shared_ptr<Label> Ptr;

    Label(const Vector2f &pos, const Vector2f &size, const std::string &text, const Color &color = Color(1.f, 1.f)) :
        Widget(pos, size),
        _text(text),
        _color(color)
    {
    }

    const std::string &text() const { return _text; }
    void setText(const std::string &text) { _text = text; }

    const Color &color() const { return _color; }
    void setColor(const Color &color) { _color = color; }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(_color);
        renderer.drawText(_pos + _size * 0.5f, _text);
    }

private:
    std::string _text;
    Color _color;
};

} // namespace sim
