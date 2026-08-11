# Purpose: Defines the CMake build graph for the unit test.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
register_test(TestMovingAverage TestMovingAverage.cpp)
register_test(TestObjectPool TestObjectPool.cpp)
register_test(TestRandom TestRandom.cpp)
register_test(TestStringUtils TestStringUtils.cpp)
