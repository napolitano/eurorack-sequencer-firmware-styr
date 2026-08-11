/**
 * @file Window.h
 * @brief Declares the Window component used by the simulator frontend.
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
#include "Widget.h"
#include "Renderer.h"

namespace sim {

class Window {
public:
    typedef std::shared_ptr<Window> Ptr;

    Window(const std::string &title, const Vector2i &size);
    ~Window();

    void close() { _terminate = true; }

    bool terminate() const { return _terminate; }

    void update();
    void render();

    template<typename T, typename... Args>
    std::shared_ptr<T> createWidget(Args... args) {
        std::shared_ptr<T> widget = std::make_shared<T>(args...);
        addWidget(widget);
        return widget;
    }

    void addWidget(Widget::Ptr widget);
    void removeWidget(Widget::Ptr widget);

private:
    template<typename Event, typename Handler>
    void handleEvent(Event event, Handler handler) {
        for (const auto &widget : _widgets) {
            handler(*widget, event);
            if (event.consumed()) {
                break;
            }
        }
    }

    SDL_Window *_window;
    Vector2i _fixedSize;
    Renderer _renderer;
    std::vector<Widget::Ptr> _widgets;
    bool _terminate = false;
};

} // namespace sim
