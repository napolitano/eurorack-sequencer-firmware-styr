/**
 * @file sequencer.cpp
 * @brief Implements the sequencer component used by the simulator Python integration.
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
#include "SequencerApp.h"
#include "model/Model.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

void register_project(py::module &m);

void register_sequencer(py::module &m) {
    // ------------------------------------------------------------------------
    // Sequencer
    // ------------------------------------------------------------------------

    py::class_<SequencerApp> sequencer(m, "Sequencer");
    sequencer
        .def_property_readonly("model", [] (SequencerApp &app) { return &app.model; })
    ;

    // ------------------------------------------------------------------------
    // Model
    // ------------------------------------------------------------------------

    py::class_<Model> model(m, "Model");
    model
        .def_property_readonly("project", [] (Model &model) { return &model.project(); })
    ;

    register_project(m);
}
