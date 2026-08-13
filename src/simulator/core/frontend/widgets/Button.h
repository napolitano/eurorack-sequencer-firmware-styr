/**
 * @file Button.h
 * @brief Declares the Button component used by the simulator frontend.
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
 * @brief Represents the button component and its associated state.
 */
class Button : public Widget {
public:
    typedef std::shared_ptr<Button> Ptr;

    /**
     * @brief Enumerates the supported shape values.
     */
    enum Shape {
        Rectangle, ///< Selects the rectangle behavior.
        Ellipse, ///< Selects the ellipse behavior.
    };

    /**
     * @brief Constructs a Button instance.
     *
     * @param[in] pos Pos supplied to the operation.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] shape Shape supplied to the operation.
     * @param[in] keycode Keycode supplied to the operation.
     */
    Button(const Vector2f &pos, const Vector2f &size, Shape shape = Rectangle, SDL_Keycode keycode = -1) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the shape.
         */
        _shape(shape),
        /**
         * @brief Returns the keycode.
         */
        _keycode(keycode)
    {}

    // bool pressed() const { return _pressed; }

    /**
     * @brief Returns the state.
     *
     * @return `true` when state; otherwise `false`.
     */
    bool state() const { return _state; }
    /**
     * @brief Sets the state.
     *
     * @param[in] state State to apply or inspect.
     */
    void setState(bool state) { _state = state; }

    /**
     * @brief Sets the callback.
     *
     * @param[in] callback Callback supplied to the operation.
     */
    void setCallback(std::function<void(bool)> callback) {
        _callback = callback;
    }

    /**
     * @brief Updates the Button for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Performs the render operation for this Button.
     *
     * @param[in] renderer Renderer supplied to the operation.
     */
    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));
        switch (_shape) {
        case Rectangle: renderer.drawRect(_pos, _size);     break;
        case Ellipse:   renderer.drawEllipse(_pos, _size);  break;
        }

        if (_state) {
            renderer.setColor(Color(1.f, 1.f));
            switch (_shape) {
            case Rectangle: renderer.fillRect(_pos + Vector2f(1, 1), _size - Vector2f(2, 2));       break;
            case Ellipse:   renderer.fillEllipse(_pos + Vector2f(1, 1), _size - Vector2f(2, 2));    break;
            }
        }
    }

    /**
     * @brief Performs the on key down operation for this Button.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onKeyDown(KeyEvent &e) override {
        if (!_pressed && e.keycode() == _keycode) {
            setPressed(true);
            e.consume();
        }
    }

    /**
     * @brief Performs the on key up operation for this Button.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onKeyUp(KeyEvent &e) override {
        if (_pressed && e.keycode() == _keycode) {
            setPressed(false);
            e.consume();
        }
    }

    /**
     * @brief Performs the on mouse move operation for this Button.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseMove(MouseMoveEvent &e) override {
        _hovered = isInside(e.pos());
    }

    /**
     * @brief Performs the on mouse down operation for this Button.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseDown(MouseButtonEvent &e) override {
        if (!_pressed && e.button() == MouseButtonEvent::Left && isInside(e.pos())) {
            setPressed(true);
            e.consume();
        }
    }

    /**
     * @brief Performs the on mouse up operation for this Button.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseUp(MouseButtonEvent &e) override {
        if (_pressed && e.button() == MouseButtonEvent::Left) {
            setPressed(false);
        }
    }

private:
    /**
     * @brief Sets the pressed.
     *
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    void setPressed(bool pressed) {
        if (pressed != _pressed) {
            _pressed = pressed;
            if (_callback) {
                _callback(_pressed);
            }
        }
    }

    Shape _shape; ///< Shape maintained by this component.
    SDL_Keycode _keycode; ///< Keycode maintained by this component.
    /**
     * @brief Whether pressed is true in the current state.
     */
    bool _pressed = false; ///< Whether pressed is active or enabled.
    /**
     * @brief Whether hovered is true in the current state.
     */
    bool _hovered = false; ///< Whether hovered is active or enabled.
    /**
     * @brief Whether state is true in the current state.
     */
    bool _state = false; ///< Whether state is active or enabled.
    /**
     * @brief Computes the void result.
     *
     * @note Includes an unnamed `bool` input parameter as declared by the inherited/interface signature.
     *
     * @return Current void.
     */
    std::function<void(bool)> _callback; ///< Callback invoked when the simulated button's pressed state changes.
};

} // namespace sim
