/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the simulator frontend.
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
 * @brief Represents the encoder component and its associated state.
 */
class Encoder : public Widget {
public:
    typedef std::shared_ptr<Encoder> Ptr;

    /**
     * @brief Constructs a Encoder instance.
     *
     * @param[in] pos Pos supplied to the operation.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] keycode Keycode supplied to the operation.
     */
    Encoder(const Vector2f &pos, const Vector2f &size, SDL_Keycode keycode = -1) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the keycode.
         */
        _keycode(keycode)
    {}

    /**
     * @brief Returns the pressed.
     *
     * @return `true` when pressed; otherwise `false`.
     */
    bool pressed() const { return _pressed; }

    /**
     * @brief Sets the button callback.
     *
     * @param[in] callback Callback supplied to the operation.
     */
    void setButtonCallback(std::function<void(bool)> callback) {
        _buttonCallback = callback;
    }

    /**
     * @brief Sets the value callback.
     *
     * @param[in] callback Callback supplied to the operation.
     */
    void setValueCallback(std::function<void(int)> callback) {
        _valueCallback = callback;
    }

    /**
     * @brief Updates the Encoder for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Performs the render operation for this Encoder.
     *
     * @param[in] renderer Renderer supplied to the operation.
     */
    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));

        if (_pressed) {
            renderer.setColor(Color(1.f, 1.f));
        }

        Vector2f center = _pos + 0.5f * _size;
        float radius = 0.5f * std::max(_size.x(), _size.y()) - 2.f;

        auto pointOnCircle = [&] (float theta) -> Vector2f {
            return center + radius * Vector2f(std::sin(theta), -std::cos(theta));
        };

        renderer.drawEllipse(_pos, _size);
        renderer.drawLine(center, pointOnCircle(_value * TWO_PI / Ticks));
    }

    /**
     * @brief Performs the on key down operation for this Encoder.
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
     * @brief Performs the on key up operation for this Encoder.
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
     * @brief Performs the on mouse move operation for this Encoder.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseMove(MouseMoveEvent &e) override {
        _hovered = isInside(e.pos());
        if (_pressed) {
            int delta = e.pos().x() - _lastPos.x();
            if (delta / 10 != 0) {
                setValue(_value + delta / 10);
                _lastPos = e.pos();
            }
        }
    }

    /**
     * @brief Performs the on mouse down operation for this Encoder.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseDown(MouseButtonEvent &e) override {
        if (!_pressed && e.button() == MouseButtonEvent::Left && isInside(e.pos())) {
            SDL_CaptureMouse(SDL_TRUE);
            _lastPos = e.pos();
            setPressed(true);
            e.consume();
        }
    }

    /**
     * @brief Performs the on mouse up operation for this Encoder.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseUp(MouseButtonEvent &e) override {
        if (_pressed && e.button() == MouseButtonEvent::Left) {
            SDL_CaptureMouse(SDL_FALSE);
            setPressed(false);
            e.consume();
        }
    }

    /**
     * @brief Performs the on mouse wheel operation for this Encoder.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseWheel(MouseWheelEvent &e) override {
        _deltaValue += e.scroll().y();
        setValue(_value + _deltaValue / ScrollDivider);
        _deltaValue %= ScrollDivider;
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
            if (_buttonCallback) {
                _buttonCallback(_pressed);
            }
        }
    }

    /**
     * @brief Sets the value.
     *
     * @param[in] value Value to apply.
     */
    void setValue(int value) {
        if (value != _value) {
            int delta = value - _value;
            _value = value;
            if (_valueCallback) {
                _valueCallback(delta);
            }
        }
    }

    /**
     * @brief Scroll divider constant used by this component.
     */
    static const int ScrollDivider = 4; ///< Scroll divider used by this component.
    /**
     * @brief Ticks constant used by this component.
     */
    static const int Ticks = 24; ///< Ticks used by this component.

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
     * @brief Simulator value representing value.
     */
    int _value = 0; ///< Value maintained by this component.
    /**
     * @brief Simulator value representing delta value.
     */
    int _deltaValue = 0; ///< Delta value maintained by this component.
    /**
     * @brief Most recently observed pos.
     */
    Vector2i _lastPos; ///< Last pos maintained by this component.

    /**
     * @brief Computes the void result.
     *
     * @note Includes an unnamed `bool` input parameter as declared by the inherited/interface signature.
     *
     * @return Current void.
     */
    std::function<void(bool)> _buttonCallback; ///< Callback invoked when the encoder push button changes state.
    /**
     * @brief Computes the void result.
     *
     * @note Includes an unnamed `int` input parameter as declared by the inherited/interface signature.
     *
     * @return Current void.
     */
    std::function<void(int)> _valueCallback; ///< Callback invoked with relative encoder movement.
};

} // namespace sim
