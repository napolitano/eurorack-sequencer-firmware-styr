# Purpose: Builds simulator-specific interactive integration-test executables.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

function(register_simulator_integration_test test file)
    add_executable(${test} ${file})
    target_include_directories(${test} PRIVATE ${STYR_TEST_FRAMEWORK_DIR})
    # IntegrationTestRunner owns a sim::Frontend, so these deliberately
    # interactive tests must consume the frontend target. Besides the actual
    # frontend implementation this propagates its SDL/OpenGL include/link
    # requirements; linking only styr_runtime breaks as soon as the headless
    # runtime is correctly isolated from desktop graphics dependencies.
    target_link_libraries(${test} PRIVATE styr_frontend)
    platform_postprocess_frontend_executable(${test})
endfunction()

register_simulator_integration_test(TestButtonLedMatrix drivers/TestButtonLedMatrix.cpp)
register_simulator_integration_test(TestAdc drivers/TestAdc.cpp)
register_simulator_integration_test(TestDac drivers/TestDac.cpp)
register_simulator_integration_test(TestDio drivers/TestDio.cpp)
register_simulator_integration_test(TestEncoder drivers/TestEncoder.cpp)
register_simulator_integration_test(TestGateOutput drivers/TestGateOutput.cpp)
register_simulator_integration_test(TestLcd drivers/TestLcd.cpp)
register_simulator_integration_test(TestMidi drivers/TestMidi.cpp)
register_simulator_integration_test(TestSdCard drivers/TestSdCard.cpp)
register_simulator_integration_test(TestUsbMidi drivers/TestUsbMidi.cpp)
register_simulator_integration_test(TestFileSystem fs/TestFileSystem.cpp)
