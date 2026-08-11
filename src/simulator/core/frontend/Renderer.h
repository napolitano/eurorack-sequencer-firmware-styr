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

class Renderer {
public:
    Renderer(SDL_Window *window);
    ~Renderer();

    NVGcontext *nvg() const { return _nvg; }

    void clear() ;
    void setColor(const Color &color);
    void drawRect(const Vector2f &pos, const Vector2f &size);
    void fillRect(const Vector2f &pos, const Vector2f &size);
    void drawEllipse(const Vector2f &pos, const Vector2f &size);
    void fillEllipse(const Vector2f &pos, const Vector2f &size);
    void drawLine(const Vector2f &p1, const Vector2f &p2);
    void drawArc(const Vector2f &pos, const Vector2f &size, float rotation);

    void drawText(const Vector2f &pos, const std::string &text);

    void present();

private:
    SDL_Window *_window;
    SDL_GLContext _context;
    NVGcontext *_nvg;
    int _font;
};

} // namespace sim
