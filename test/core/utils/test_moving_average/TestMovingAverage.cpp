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
#include <unity.h>

#include "core/utils/MovingAverage.h"

#include <array>

#include <cmath>
#include <cstdint>

void test_01_initialized_to_zero() {
        MovingAverage<int, 4> ma;
        TEST_ASSERT_TRUE((ma()) == (0));
    
}

void test_02_constant_average() {
        MovingAverage<int, 4> ma;
        for (size_t i = 0; i < 8; ++i) {
            ma.push(10);
            TEST_ASSERT_TRUE((ma()) == (10));
        }
    
}

void test_03_int_sequence() {
        MovingAverage<int, 4> ma;
        for (size_t i = 0; i < 4; ++i) {
            ma.push(10);
        }
        TEST_ASSERT_TRUE((ma()) == (10));
        for (size_t i = 0; i < 2; ++i) {
            ma.push(20);
        }
        TEST_ASSERT_TRUE((ma()) == (15));
        for (size_t i = 0; i < 2; ++i) {
            ma.push(20);
        }
        TEST_ASSERT_TRUE((ma()) == (20));
    
}

void test_04_float_sequence() {
        MovingAverage<float, 2> ma;
        TEST_ASSERT_TRUE((ma()) == (0.f));
        ma.push(0.f);
        TEST_ASSERT_TRUE((ma()) == (0.f));
        ma.push(1.f);
        TEST_ASSERT_TRUE((ma()) == (0.5f));
        ma.push(1.f);
        TEST_ASSERT_TRUE((ma()) == (1.f));
        ma.push(0.f);
        TEST_ASSERT_TRUE((ma()) == (0.5f));
        ma.push(0.f);
        TEST_ASSERT_TRUE((ma()) == (0.f));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_initialized_to_zero);
    RUN_TEST(test_02_constant_average);
    RUN_TEST(test_03_int_sequence);
    RUN_TEST(test_04_float_sequence);
    return UNITY_END();
}

