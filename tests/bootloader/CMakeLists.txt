# Purpose: Standalone host build for release-critical bootloader tests.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
project(StyrBootloaderHostTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(STYR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# The same unit targets are also included by the full simulator test graph.
# Standalone bootloader tests have no simulator runtime or DLL staging needs.
function(platform_postprocess_executable executable)
endfunction()

enable_testing()
add_subdirectory("${STYR_ROOT}/tests/unit/bootloader" bootloader)
