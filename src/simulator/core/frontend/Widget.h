/**
 * @file Widget.h
 * @brief Declares the Widget component used by the simulator frontend.
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
#include "Renderer.h"
#include "Event.h"

namespace sim {

/**
 * @brief Provides widget behavior for the desktop simulator.
 */
class Widget {
public:
    typedef std::shared_ptr<Widget> Ptr;

    /**
     * @brief Constructs a Widget instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    Widget(const Vector2f &pos, const Vector2f &size) :
        /**
         * @brief Returns the pos.
         */
        _pos(pos),
        /**
         * @brief Returns the size.
         */
        _size(size)
    {}

    /**
     * @brief Destroys the Widget instance.
     */
    virtual ~Widget() {}

    /**
     * @brief Returns the pos.
     *
     * @return Reference to the pos.
     */
    const Vector2f &pos() const { return _pos; }
          /**
           * @brief Returns the pos.
           *
           * @return Reference to the pos.
           */
          Vector2f &pos()       { return _pos; }

    /**
     * @brief Returns the size.
     *
     * @return Reference to the size.
     */
    const Vector2f &size() const { return _size; }
          /**
           * @brief Returns the size.
           *
           * @return Reference to the size.
           */
          Vector2f &size()       { return _size; }

    /**
     * @brief Reports whether inside.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     *
     * @return `true` if inside; otherwise `false`.
     */
    bool isInside(const Vector2f &pos) const {
        return (
            pos.x() >= _pos.x() &&
            pos.x() <= _pos.x() + _size.x() &&
            pos.y() >= _pos.y() &&
            pos.y() <= _pos.y() + _size.y()
        );
    }

    /**
     * @brief Reports whether inside.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     *
     * @return `true` if inside; otherwise `false`.
     */
    bool isInside(const Vector2i &pos) const {
        return (
            pos.x() >= _pos.x() &&
            pos.x() <= _pos.x() + _size.x() &&
            pos.y() >= _pos.y() &&
            pos.y() <= _pos.y() + _size.y()
        );
    }

    /**
     * @brief Updates the Widget for the current service cycle.
     */
    virtual void update() = 0;
    /**
     * @brief Renders this component into the supplied target.
     *
     * @param[in] renderer Renderer used to create or update frontend resources.
     */
    virtual void render(Renderer &renderer) = 0;

    /**
     * @brief Handles the key down callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onKeyDown(KeyEvent &e) {}
    /**
     * @brief Handles the key up callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onKeyUp(KeyEvent &e) {}
    /**
     * @brief Handles the mouse move callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onMouseMove(MouseMoveEvent &e) {}
    /**
     * @brief Handles the mouse down callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onMouseDown(MouseButtonEvent &e) {}
    /**
     * @brief Handles the mouse up callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onMouseUp(MouseButtonEvent &e) {}
    /**
     * @brief Handles the mouse wheel callback.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    virtual void onMouseWheel(MouseWheelEvent &e) {}

protected:
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    Vector2f _pos; ///< Current read/write position within the active buffer/stream.
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    Vector2f _size; ///< Size of the associated data in bytes/elements as defined by this type.
};

} // namespace sim
