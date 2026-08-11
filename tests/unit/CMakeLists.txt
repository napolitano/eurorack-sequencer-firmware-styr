# Purpose: Defines the CMake build graph for the unit test.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
function(register_test test file)
    add_executable(${test} ${file})
    target_include_directories(${test} PRIVATE ${STYR_TEST_FRAMEWORK_DIR} ${STYR_SEQUENCER_DIR} ${STYR_SHARED_DIR})
    target_link_libraries(${test} PRIVATE styr_runtime ${ARGN})
    platform_postprocess_executable(${test})
    if(NOT CMAKE_CROSSCOMPILING)
        add_test(NAME ${test} COMMAND ${test})
    endif()
endfunction()

add_subdirectory(core)
add_subdirectory(sequencer)
