/**
 * @file simulator.cpp
 * @brief Implements the simulator component used by the simulator Python integration.
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
#include "core/Simulator.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

using namespace sim;

void register_simulator(py::module &m) {
    // ------------------------------------------------------------------------
    // Simulator
    // ------------------------------------------------------------------------

    py::class_<Simulator> simulator(m, "Simulator", py::dynamic_attr());
    simulator
        .def("wait", &Simulator::wait)
        .def("setButton", &Simulator::setButton)
        .def("setEncoder", &Simulator::setEncoder)
        .def("rotateEncoder", &Simulator::rotateEncoder)
        .def("setAdc", &Simulator::setAdc)
        .def("setDio", &Simulator::setDio)
        .def("sendMidi", &Simulator::sendMidi)
        .def("screenshot", &Simulator::screenshot, py::arg("filename"), py::arg("scale") = 1)
        .def_property_readonly("targetState", &Simulator::targetState, py::return_value_policy::reference)
    ;

    // ------------------------------------------------------------------------
    // TargetTrace
    // ------------------------------------------------------------------------

    py::class_<TargetTrace> trace(m, "TargetTrace", py::dynamic_attr());
    trace
        .def(py::init<>())

        .def("saveToFile", &TargetTrace::saveToFile)
        .def("loadFromFile", &TargetTrace::loadFromFile)
        .def("saveToText", &TargetTrace::saveToText)
    ;
}
