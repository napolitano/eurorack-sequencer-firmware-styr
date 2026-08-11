/**
 * @file Intro.h
 * @brief Declares the Intro component used by the sequencer startup introduction.
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

#include "core/gfx/Canvas.h"

class Intro {
public:
    Intro();

    void init();

    void update(float dt);
    void draw(Canvas &canvas);

private:
    float _time;
};
