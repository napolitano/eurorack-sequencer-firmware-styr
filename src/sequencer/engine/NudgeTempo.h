/**
 * @file NudgeTempo.h
 * @brief Declares the NudgeTempo component used by the sequencer engine.
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

#include "core/math/Math.h"

#include <cstdint>

/**
 * @brief Applies a temporary directional tempo offset with a smoothly varying strength.
 */
class NudgeTempo {
public:
    /**
     * @brief Constructs a NudgeTempo instance.
     */
    NudgeTempo() {
    }

    /**
     * @brief Sets the direction.
     *
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     */
    void setDirection(int direction) {
        _direction = direction;
    }

    /**
     * @brief Returns the strength.
     *
     * @return Current signed tempo-nudge strength.
     */
    float strength() const {
        return _strength;
    }

    /**
     * @brief Updates the NudgeTempo for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    void update(float dt) {
        if (_direction == 0) {
            _strength = 0.f;
        } else {
            _strength = clamp(_strength + _direction * dt, -1.f, 1.f);
        }
    }

private:
    /**
     * @brief Runtime value representing direction.
     */
    int8_t _direction = 0; ///< Direction selector or signed direction stored by this state.
    /**
     * @brief Runtime value representing strength.
     */
    float _strength = 0.f; ///< Current normalized tempo-nudge strength applied in the selected direction.
};
