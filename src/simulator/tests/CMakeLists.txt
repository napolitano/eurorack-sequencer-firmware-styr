# Purpose: Defines simulator-only tests and host integration harnesses.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
#
# Product/firmware unit tests are deliberately NOT part of this CMake graph.
# They are owned by PlatformIO and live below the repository's test/ tree.

function(register_simulator_test test file)
    add_executable(${test} ${file})
    target_include_directories(${test} PRIVATE
        ${STYR_TEST_FRAMEWORK_DIR}
        ${STYR_SEQUENCER_DIR}
        ${STYR_SHARED_DIR}
    )
    target_link_libraries(${test} PRIVATE styr_runtime styr_sequencer)
    platform_postprocess_executable(${test})
    if(NOT CMAKE_CROSSCOMPILING)
        add_test(NAME ${test} COMMAND ${test})
    endif()
endfunction()

# This test is specifically about simulator target recreation and host-side
# observer/callback lifecycle behavior, so CTest remains the correct owner.
register_simulator_test(TestSimulatorReboot unit/TestSimulatorReboot.cpp)
register_simulator_test(TestClockTimer unit/TestClockTimer.cpp)

add_subdirectory(integration)
