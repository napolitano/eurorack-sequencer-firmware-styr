/**
 * @file Event.h
 * @brief Declares the Event component used by the simulator frontend.
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

namespace sim {

/**
 * @brief Provides event behavior for the desktop simulator.
 */
class Event {
public:
    /**
     * @brief Returns the consumed.
     *
     * @return `true` if consumed; otherwise `false`.
     */
    bool consumed() const { return _consumed; }
    /**
     * @brief Consumes the current buffered event/value.
     */
    void consume() { _consumed = true; }

private:
    /**
     * @brief Whether consumed is true in the current state.
     */
    bool _consumed = false; ///< True after an event handler has consumed the event and propagation should stop.
};

/**
 * @brief Provides key event behavior for the desktop simulator.
 */
class KeyEvent : public Event {
public:
    /**
     * @brief Constructs a KeyEvent instance.
     *
     * @param[in] scancode SDL/platform physical-key scancode associated with the event.
     * @param[in] keycode Platform key code associated with the input event.
     * @param[in] modifiers Keyboard modifier bit mask associated with the event.
     */
    KeyEvent(SDL_Scancode scancode, SDL_Keycode keycode, uint16_t modifiers) :
        /**
         * @brief Returns the scancode.
         */
        _scancode(scancode),
        /**
         * @brief Returns the keycode.
         */
        _keycode(keycode),
        /**
         * @brief Returns the modifiers.
         */
        _modifiers(modifiers)
    {}

    /**
     * @brief Returns the scancode.
     *
     * @return Platform scan code carried by the keyboard event.
     */
    SDL_Scancode scancode() const { return _scancode; }
    /**
     * @brief Returns the keycode.
     *
     * @return Logical key code carried by the keyboard event.
     */
    SDL_Keycode keycode() const { return _keycode; }
    /**
     * @brief Returns the modifiers.
     *
     * @return Modifier-key bit mask carried by the keyboard event.
     */
    uint16_t modifiers() const { return _modifiers; }

    /**
     * @brief Returns from sdl.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     *
     * @return Frontend event/value converted from the SDL representation.
     */
    static KeyEvent fromSDL(const SDL_KeyboardEvent &e) {
        return KeyEvent(e.keysym.scancode, e.keysym.sym, e.keysym.mod);
    }

private:
    SDL_Scancode _scancode; ///< SDL keyboard scancode associated with this frontend keyboard event.
    SDL_Keycode _keycode; ///< SDL key code associated with the input state.
    /**
     * @brief Simulator value representing modifiers.
     */
    uint16_t _modifiers; ///< Bit mask of keyboard modifier keys active for this frontend event.
};

/**
 * @brief Provides mouse move event behavior for the desktop simulator.
 */
class MouseMoveEvent : public Event {
public:
    /**
     * @brief Constructs a MouseMoveEvent instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] rel Relative pointer/mouse motion since the previous event.
     */
    MouseMoveEvent(const Vector2i &pos, const Vector2i &rel) :
        /**
         * @brief Returns the pos.
         */
        _pos(pos),
        /**
         * @brief Returns the rel.
         */
        _rel(rel)
    {}

    /**
     * @brief Returns the pos.
     *
     * @return Reference to the pos.
     */
    const Vector2i &pos() const { return _pos; }
    /**
     * @brief Returns the rel.
     *
     * @return Reference to the rel.
     */
    const Vector2i &rel() const { return _rel; }

    /**
     * @brief Returns from sdl.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     *
     * @return Frontend event/value converted from the SDL representation.
     */
    static MouseMoveEvent fromSDL(const SDL_MouseMotionEvent &e) {
        return MouseMoveEvent(Vector2i(e.x, e.y), Vector2i(e.xrel, e.yrel));
    }

private:
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    Vector2i _pos; ///< Current read/write position within the active buffer/stream.
    Vector2i _rel; ///< Relative mouse-motion vector associated with this frontend event.
};

/**
 * @brief Provides mouse button event behavior for the desktop simulator.
 */
class MouseButtonEvent : public Event {
public:
    /**
     * @brief Enumerates the supported button values.
     */
    enum Button {
        Left, ///< Selects the left button.
        Middle, ///< Selects the middle button.
        Right, ///< Selects the right button.
        Unknown, ///< Selects the unknown button.
    };

    /**
     * @brief Constructs a MouseButtonEvent instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] clicks Number of clicks reported by the pointer/button event.
     */
    MouseButtonEvent(const Vector2i &pos, Button button, int clicks) :
        /**
         * @brief Returns the pos.
         */
        _pos(pos),
        /**
         * @brief Returns the button.
         */
        _button(button),
        /**
         * @brief Returns the clicks.
         */
        _clicks(clicks)
    {}

    /**
     * @brief Returns the pos.
     *
     * @return Reference to the pos.
     */
    const Vector2i &pos() const { return _pos; }
    /**
     * @brief Returns the button.
     *
     * @return Mouse/controller button identifier carried by the event.
     */
    Button button() const { return _button; }
    /**
     * @brief Returns the clicks.
     *
     * @return Click count carried by the pointer event.
     */
    int clicks() const { return _clicks; }

    /**
     * @brief Returns from sdl.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     *
     * @return Frontend event/value converted from the SDL representation.
     */
    static MouseButtonEvent fromSDL(const SDL_MouseButtonEvent &e) {
        auto button = [] (uint8_t button) {
            switch (button) {
            case SDL_BUTTON_LEFT: return Left;
            case SDL_BUTTON_MIDDLE: return Middle;
            case SDL_BUTTON_RIGHT: return Right;
            default: return Unknown;
            }
        };
        return MouseButtonEvent(Vector2i(e.x, e.y), button(e.button), e.clicks);
    }

private:
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    Vector2i _pos; ///< Current read/write position within the active buffer/stream.
    Button _button; ///< Mouse button identifier associated with this frontend event.
    /**
     * @brief Simulator value representing clicks.
     */
    int _clicks; ///< Number of mouse clicks represented by this frontend event.
};

/**
 * @brief Provides mouse wheel event behavior for the desktop simulator.
 */
class MouseWheelEvent : public Event {
public:
    /**
     * @brief Constructs a MouseWheelEvent instance.
     *
     * @param[in] scroll Scroll delta reported by the input event.
     */
    MouseWheelEvent(const Vector2i &scroll) :
        /**
         * @brief Returns the scroll.
         */
        _scroll(scroll)
    {}

    /**
     * @brief Returns the scroll.
     *
     * @return Reference to the scroll.
     */
    const Vector2i &scroll() const { return _scroll; }

    /**
     * @brief Returns from sdl.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     *
     * @return Frontend event/value converted from the SDL representation.
     */
    static MouseWheelEvent fromSDL(const SDL_MouseWheelEvent &e) {
        return MouseWheelEvent(Vector2i(e.x, e.y));
    }

private:
    Vector2i _scroll; ///< Horizontal/vertical scroll delta associated with this frontend event.
};

} // namespace sim
