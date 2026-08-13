/**
 * @file Rotary.h
 * @brief Declares the Rotary component used by the simulator frontend.
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

#include <cmath>

namespace sim {

/**
 * @brief Represents the rotary component and its associated state.
 */
class Rotary : public Widget {
public:
    typedef std::shared_ptr<Rotary> Ptr;

    /**
     * @brief Constructs a Rotary instance.
     *
     * @param[in] pos Pos supplied to the operation.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    Rotary(const Vector2f &pos, const Vector2f &size) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size)
    {}

    /**
     * @brief Returns the value.
     *
     * @return The value value.
     */
    float value() const { return _value; }
    /**
     * @brief Sets the value.
     *
     * @param[in] value Value to apply.
     */
    void setValue(float value) { _value = std::max(0.f, std::min(1.f, value)); }

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
    void setValueCallback(std::function<void(float)> callback) {
        _valueCallback = callback;
    }

    /**
     * @brief Updates the Rotary for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Performs the render operation for this Rotary.
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
        renderer.drawLine(center, pointOnCircle(-PI * 0.75f + _value * TWO_PI * 0.75f));
    }

    /**
     * @brief Performs the on mouse move operation for this Rotary.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseMove(MouseMoveEvent &e) override {
        _hovered = isInside(e.pos());
        if (_pressed) {
            int delta = e.pos().x() - _lastPos.x();
            float nextValue = _value + delta / 100.f;

            // Hold Shift while dragging to quantize to 0.25V steps.
            // Rotary uses a normalized [0..1] range that maps to [-5V..+5V],
            // so one 0.25V step equals 0.025 in normalized units.
            if ((SDL_GetModState() & KMOD_SHIFT) != 0) {
                constexpr float normalizedStep = 0.025f;
                nextValue = std::round(nextValue / normalizedStep) * normalizedStep;
            }

            updateValue(nextValue);
            _lastPos = e.pos();
        }
    }

    /**
     * @brief Performs the on mouse down operation for this Rotary.
     *
     * @param[in] e E supplied to the operation.
     */
    virtual void onMouseDown(MouseButtonEvent &e) override {
        if (!_pressed && e.button() == MouseButtonEvent::Left && isInside(e.pos())) {
            if (e.clicks() >= 2) {
                // Double-click resets rotary center (exactly 0V in CV mapping).
                updateValue(0.5f);
                e.consume();
                return;
            }
            SDL_CaptureMouse(SDL_TRUE);
            _lastPos = e.pos();
            setPressed(true);
            e.consume();
        }
    }

    /**
     * @brief Performs the on mouse up operation for this Rotary.
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
     * @brief Updates the value.
     *
     * @param[in] value Value to apply.
     */
    void updateValue(float value) {
        value = std::max(0.f, std::min(1.f, value));
        if (value != _value) {
            _value = value;
            if (_valueCallback) {
                _valueCallback(value);
            }
        }
    }

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
    float _value = 0.5f; ///< Value maintained by this component.
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
    std::function<void(bool)> _buttonCallback; ///< Callback invoked when the rotary push button changes state.
    /**
     * @brief Computes the void result.
     *
     * @note Includes an unnamed `float` input parameter as declared by the inherited/interface signature.
     *
     * @return Current void.
     */
    std::function<void(float)> _valueCallback; ///< Callback invoked with the rotary control's normalized value.
};

} // namespace sim
