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

/**
 * @brief Provides window behavior for the desktop simulator.
 */
class Window {
public:
    typedef std::shared_ptr<Window> Ptr;

    /**
     * @brief Constructs a Window instance.
     *
     * @param[in] title Title text displayed by the UI.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    Window(const std::string &title, const Vector2i &size);
    /**
     * @brief Destroys the Window instance.
     */
    ~Window();

    /**
     * @brief Closes the active page, file, dialog, or resource.
     */
    void close() { _terminate = true; }

    /**
     * @brief Returns the terminate.
     *
     * @return `true` if terminate; otherwise `false`.
     */
    bool terminate() const { return _terminate; }

    /**
     * @brief Updates the Window for the current service cycle.
     */
    void update();
    /**
     * @brief Renders this component into the supplied target.
     */
    void render();

    template<typename T, typename... Args>
    /**
     * @brief Returns create widget.
     *
     * @param[in] args Argument collection forwarded to the operation.
     *
     * @return Result of createWidget().
     */
    std::shared_ptr<T> createWidget(Args... args) {
        std::shared_ptr<T> widget = std::make_shared<T>(args...);
        addWidget(widget);
        return widget;
    }

    /**
     * @brief Adds widget.
     *
     * @param[in] widget Widget instance read or modified by the frontend.
     */
    void addWidget(Widget::Ptr widget);
    /**
     * @brief Removes widget.
     *
     * @param[in] widget Widget instance read or modified by the frontend.
     */
    void removeWidget(Widget::Ptr widget);

private:
    template<typename Event, typename Handler>
    /**
     * @brief Handles event.
     *
     * @param[in] event Event to process.
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void handleEvent(Event event, Handler handler) {
        for (const auto &widget : _widgets) {
            handler(*widget, event);
            if (event.consumed()) {
                break;
            }
        }
    }

    /**
     * @brief Pointer to window; `nullptr` denotes that no object/resource is assigned.
     */
    SDL_Window *_window; ///< Pointer to window; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Size of fixed in bytes/elements as defined by this type.
     */
    Vector2i _fixedSize; ///< Size of fixed in bytes/elements as defined by this type.
    Renderer _renderer; ///< Renderer owned by the simulator window wrapper.
    std::vector<Widget::Ptr> _widgets; ///< Child widgets owned/shared by the simulator window in rendering order.
    /**
     * @brief Whether terminate is true in the current state.
     */
    bool _terminate = false; ///< True after the frontend has requested termination of the simulator window loop.
};

} // namespace sim
