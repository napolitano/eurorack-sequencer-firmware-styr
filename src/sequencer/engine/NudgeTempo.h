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

class NudgeTempo {
public:
    NudgeTempo() {
    }

    void setDirection(int direction) {
        _direction = direction;
    }

    float strength() const {
        return _strength;
    }

    void update(float dt) {
        if (_direction == 0) {
            _strength = 0.f;
        } else {
            _strength = clamp(_strength + _direction * dt, -1.f, 1.f);
        }
    }

private:
    int8_t _direction = 0;
    float _strength = 0.f;
};
