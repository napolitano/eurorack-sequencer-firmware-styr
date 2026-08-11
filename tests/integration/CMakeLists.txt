# Purpose: Defines the CMake build graph for the integration test.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
function(register_integration_test test file)
    add_executable(${test} ${file})
    target_include_directories(${test} PRIVATE ${STYR_TEST_FRAMEWORK_DIR})
    target_link_libraries(${test} PRIVATE styr_runtime)
    platform_postprocess_executable(${test})
endfunction()

register_integration_test(TestButtonLedMatrix drivers/TestButtonLedMatrix.cpp)
register_integration_test(TestAdc drivers/TestAdc.cpp)
register_integration_test(TestDac drivers/TestDac.cpp)
register_integration_test(TestDio drivers/TestDio.cpp)
register_integration_test(TestEncoder drivers/TestEncoder.cpp)
register_integration_test(TestGateOutput drivers/TestGateOutput.cpp)
register_integration_test(TestLcd drivers/TestLcd.cpp)
register_integration_test(TestMidi drivers/TestMidi.cpp)
register_integration_test(TestSdCard drivers/TestSdCard.cpp)
register_integration_test(TestUsbMidi drivers/TestUsbMidi.cpp)
register_integration_test(TestFileSystem fs/TestFileSystem.cpp)
