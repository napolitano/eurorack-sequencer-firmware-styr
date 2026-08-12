/**
 * @file TestRandom.cpp
 * @brief Implements the TestRandom component used by the unit test.
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

#include "core/utils/Random.h"

#include <array>

#include <cstdlib>
#include <cstdint>
#include <cmath>

template<uint64_t Range, uint64_t Bins>
struct Histogram {
    std::array<uint64_t, Bins> counts;

    Histogram() { counts.fill(0); }

    void push(uint64_t value) {
        uint64_t bin = value / (Range / Bins);
        ++counts[bin];
    }
};

void test_01_next_returns_uniform_distribution() {
        Random rng;
        Histogram<0x100000000, 100> histogram;
        for (size_t i = 0; i < 10000000; ++i) {
            histogram.push(rng.next());
        }
        for (const auto &count : histogram.counts) {
            TEST_ASSERT_TRUE(std::abs(100000 - int(count)) < 1000);
        }
    
}

void test_02_nextbinary_returns_uniform_distribution() {
        Random rng;
        Histogram<2, 2> histogram;
        for (size_t i = 0; i < 200000; ++i) {
            histogram.push(rng.nextBinary());
        }
        for (const auto &count : histogram.counts) {
            TEST_ASSERT_TRUE(std::abs(100000 - int(count)) < 1000);
        }
    
}

void test_03_nextrange_returns_uniform_distribution() {
        Random rng;
        Histogram<16, 16> histogram;
        for (size_t i = 0; i < 1600000; ++i) {
            histogram.push(rng.nextRange(16));
        }
        for (const auto &count : histogram.counts) {
            TEST_ASSERT_TRUE(std::abs(100000 - int(count)) < 1000);
        }
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_next_returns_uniform_distribution);
    RUN_TEST(test_02_nextbinary_returns_uniform_distribution);
    RUN_TEST(test_03_nextrange_returns_uniform_distribution);
    return UNITY_END();
}

