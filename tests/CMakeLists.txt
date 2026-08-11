# Purpose: Defines the CMake build graph for the Styr source.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
# Tests are shared between the host simulator build and the embedded build.
# The active platform supplies test/UnitTestRunner.h and test/IntegrationTestRunner.h.
add_subdirectory(integration)
add_subdirectory(unit)
