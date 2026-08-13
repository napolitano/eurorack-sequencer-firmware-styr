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

/**
 * @brief Runs the startup/intro animation state machine.
 */
class Intro {
public:
    /**
     * @brief Constructs a Intro instance.
     */
    Intro();

    /**
     * @brief Initializes the Intro and its runtime resources.
     */
    void init();

    /**
     * @brief Updates the Intro for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    void update(float dt);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    /**
     * @brief Runtime value representing time.
     */
    float _time; ///< Elapsed intro-animation time in seconds.
};
