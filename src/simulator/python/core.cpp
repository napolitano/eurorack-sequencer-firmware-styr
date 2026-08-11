/**
 * @file core.cpp
 * @brief Implements the core component used by the simulator Python integration.
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
#include "core/midi/MidiMessage.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace py::literals;

void register_core(py::module &m) {
    // ------------------------------------------------------------------------
    // MidiMessage
    // ------------------------------------------------------------------------

    py::class_<MidiMessage> midiMessage(m, "MidiMessage");
    midiMessage
        .def_static("makeNoteOff", &MidiMessage::makeNoteOff, py::arg("channel"), py::arg("note"), py::arg("velocity") = 0)
        .def_static("makeNoteOn", &MidiMessage::makeNoteOn, py::arg("channel"), py::arg("note"), py::arg("velocity") = 127)
        .def_static("makeKeyPressure", &MidiMessage::makeKeyPressure, py::arg("channel"), py::arg("note"), py::arg("pressure"))
        .def_static("makeControlChange", &MidiMessage::makeControlChange, py::arg("channel"), py::arg("controlNumber"), py::arg("controlValue"))
        .def_static("makeProgramChange", &MidiMessage::makeProgramChange, py::arg("channel"), py::arg("programNumber"))
        .def_static("makeChannelPressure", &MidiMessage::makeChannelPressure, py::arg("channel"), py::arg("pressure"))
        .def_static("makePitchBend", &MidiMessage::makePitchBend, py::arg("channel"), py::arg("pitchBend"))
    ;
}
