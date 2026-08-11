/**
 * @file main.cpp
 * @brief Implements the main component used by the desktop simulator.
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
#ifdef _WIN32
#include <SDL.h>
#endif

#include "SequencerApp.h"

#include "core/Simulator.h"
#include "core/frontend/Frontend.h"

#include <memory>

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SDL_SetMainReady();
#endif

    std::unique_ptr<SequencerApp> app;

    sim::Simulator sim({
        .create = [&] () {
            app.reset(new SequencerApp());
        },
        .destroy = [&] () {
            app.reset();
        },
        .update = [&] () {
            app->update();
        }
    });

    sim::Frontend frontend(sim);

    return frontend.main(argc, argv);
}
