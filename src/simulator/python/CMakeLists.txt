# Purpose: Defines the CMake build graph for the simulator Python integration.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
pybind11_add_module(testsim testsim.cpp core.cpp project.cpp sequencer.cpp simulator.cpp)
target_link_libraries(testsim PRIVATE styr_sequencer)
target_include_directories(testsim PRIVATE ${STYR_SEQUENCER_DIR} ${STYR_SHARED_DIR})
