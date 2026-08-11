/**
 * @file TestMovingAverage.cpp
 * @brief Implements the TestMovingAverage component used by the unit test.
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
#include "UnitTest.h"

#include "core/utils/MovingAverage.h"

#include <array>

#include <cmath>
#include <cstdint>

UNIT_TEST("MovingAverage") {

    CASE("initialized to zero") {
        MovingAverage<int, 4> ma;
        expectEqual(ma(), 0);
    }

    CASE("constant average") {
        MovingAverage<int, 4> ma;
        for (size_t i = 0; i < 8; ++i) {
            ma.push(10);
            expectEqual(ma(), 10);
        }
    }

    CASE("int sequence") {
        MovingAverage<int, 4> ma;
        for (size_t i = 0; i < 4; ++i) {
            ma.push(10);
        }
        expectEqual(ma(), 10);
        for (size_t i = 0; i < 2; ++i) {
            ma.push(20);
        }
        expectEqual(ma(), 15);
        for (size_t i = 0; i < 2; ++i) {
            ma.push(20);
        }
        expectEqual(ma(), 20);
    }

    CASE("float sequence") {
        MovingAverage<float, 2> ma;
        expectEqual(ma(), 0.f);
        ma.push(0.f);
        expectEqual(ma(), 0.f);
        ma.push(1.f);
        expectEqual(ma(), 0.5f);
        ma.push(1.f);
        expectEqual(ma(), 1.f);
        ma.push(0.f);
        expectEqual(ma(), 0.5f);
        ma.push(0.f);
        expectEqual(ma(), 0.f);
    }

}
