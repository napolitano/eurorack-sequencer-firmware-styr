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

class Panel : public Widget {
public:
    typedef std::shared_ptr<Panel> Ptr;

    Panel(const Vector2f &pos, const Vector2f &size, const Color &color) :
        Widget(pos, size),
        _color(color)
    {}

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(_color);
        renderer.fillRect(_pos, _size);
    }

private:
    Color _color;
};

} // namespace sim
