/**
 * @file Jack.h
 * @brief Declares the Jack component used by the simulator frontend.
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
 * @brief Provides jack behavior for the desktop simulator.
 */
class Jack : public Widget {
public:
    typedef std::shared_ptr<Jack> Ptr;

    /**
     * @brief Constructs a Jack instance.
     *
     * @param[in] pos Pos used by the operation.
     * @param[in] size Number of bytes or elements covered by the operation.
     *
     * @note Includes an unnamed `const Color &color = Color(0.f, 1.f)` input parameter as declared by the inherited/interface signature.
     */
    Jack(const Vector2f &pos, const Vector2f &size, const Color &color = Color(0.f, 1.f)) :
        Widget(pos, size),
        _color(color)
    {}

    /**
     * @brief Returns the color.
     *
     * @return Current drawing color/index.
     */
    const Color &color() const { return _color; }
          /**
           * @brief Returns the color.
           *
           * @return Current drawing color/index.
           */
          Color &color()       { return _color; }

    /**
     * @brief Applies a new runtime state and performs the transition-specific bookkeeping.
     *
     * @param[in] state New state or logical level to apply.
     */
    void setState(bool state) {
        _signal = State;
        _state = state;
    }

    /**
     * @brief Sets the value.
     *
     * @param[in] value Replacement value setting.
     * @param[in] min Min used by the operation.
     * @param[in] max Max used by the operation.
     */
    void setValue(float value, float min, float max) {
        _signal = Value;
        _value = value;
        _min = min;
        _max = max;
    }

    /**
     * @brief Updates the Jack for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Renders this component into the supplied target.
     *
     * @param[in] renderer Renderer used by the operation.
     */
    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawEllipse(_pos, _size);
        switch (_signal) {
        case None:
            break;
        case State:
            if (_state) {
                renderer.setColor(Color(1.f, 1.f));
                renderer.drawEllipse(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f));
            }
            break;
        case Value:
            float rotation = ((_value - _min) / (_max - _min)) * 2.f - 1.f;
            renderer.setColor(Color(1.f, 1.f));
            renderer.drawArc(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f), rotation);
            break;
        }
    }

private:
    Color _color; ///< Current drawing color/index.
    /**
     * @brief Enumerates the supported signal values.
     */
    enum Signal {
        None, ///< Represents no selection or an invalid value.
        State, ///< Selects the state signal.
        Value, ///< Selects the value signal.
    };
    Signal _signal = None; ///< Signal state owned by this component.
    bool _state; ///< Whether state is true in the current state.
    float _value; ///< Simulator value representing value.
    float _min; ///< Simulator value representing min.
    float _max; ///< Simulator value representing max.
};

} // namespace sim
