/**
 * @file TestRhythm.cpp
 * @brief Implements the TestRhythm component used by the unit test.
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

#include "engine/generators/Rhythm.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
namespace {

static int countBeats(const Rhythm::Pattern &p) {
    int n = 0;
    for (size_t i = 0; i < p.size(); ++i) {
        if (p[i]) ++n;
    }
    return n;
}

static bool patternMatches(const Rhythm::Pattern &p, const bool *expected, int len) {
    if (int(p.size()) != len) {
        return false;
    }
    for (int i = 0; i < len; ++i) {
        if (p[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

} // namespace

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

// ---- specific-pattern correctness -----------------------------------------------

void test_01_euclidean_3_8_returns_classic_tresillo_1_0_0_1_0_0_1_0() {
        // Path: loop starts with xCount(3) < yCount(5), then xCount(3) >= yCount(2);
        // exits because yCount reaches 1.
        const bool expected[] = {1,0,0,1,0,0,1,0};
        auto p = Rhythm::euclidean(3, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (3));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 8));
    
}

void test_02_euclidean_5_8_returns_1_0_1_1_0_1_1_0() {
        // Path: loop starts with xCount(5) >= yCount(3);
        // exercises the if-branch in the first iteration.
        const bool expected[] = {1,0,1,1,0,1,1,0};
        auto p = Rhythm::euclidean(5, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (5));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 8));
    
}

void test_03_euclidean_4_16_returns_four_evenly_spaced_beats() {
        // Path: xCount(4) < yCount(12) for two iterations, then xCount >= yCount;
        // covers the else-branch being taken multiple times before the if-branch.
        const bool expected[] = {1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0};
        auto p = Rhythm::euclidean(4, 16);
        TEST_ASSERT_TRUE((int(p.size())) == (16));
        TEST_ASSERT_TRUE((countBeats(p)) == (4));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 16));
    
}

void test_04_euclidean_7_8_returns_1_0_1_1_1_1_1_1() {
        // Path: xCount(7) >= yCount(1) → loop exits immediately because
        // xCount becomes 1 (not > 1) after a single iteration.
        const bool expected[] = {1,0,1,1,1,1,1,1};
        auto p = Rhythm::euclidean(7, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (7));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 8));
    
}

void test_05_euclidean_1_8_returns_1_0_0_0_0_0_0_0() {
        // Path: xCount(1) < yCount(7) → loop exits immediately because
        // xCount is 1 (not > 1) after a single iteration.
        const bool expected[] = {1,0,0,0,0,0,0,0};
        auto p = Rhythm::euclidean(1, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (1));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 8));
    
}

void test_06_euclidean_5_13_multi_step_mixed_path() {
        // Path: else-branch (5<8), if-branch (5>3), if-branch (3>2);
        // exercises both branches across multiple iterations.
        const bool expected[] = {1,0,0,1,0,1,0,0,1,0,1,0,0};
        auto p = Rhythm::euclidean(5, 13);
        TEST_ASSERT_TRUE((int(p.size())) == (13));
        TEST_ASSERT_TRUE((countBeats(p)) == (5));
        TEST_ASSERT_TRUE(patternMatches(p, expected, 13));
    
}

// ---- boundary / edge cases -------------------------------------------------------

void test_07_euclidean_0_8_returns_all_silent_pattern_of_size_8() {
        // Path: beats clamped to 0; xCount=0 forces loop to exit immediately
        // (0 > 1 is false) after one body execution.
        auto p = Rhythm::euclidean(0, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (0));
        for (int i = 0; i < 8; ++i) {
            TEST_ASSERT_FALSE(p[i]);
        }
    
}

void test_08_euclidean_8_8_returns_all_active_pattern_of_size_8() {
        // Path: yCount=0 from the start; xCount becomes 0 after one body
        // execution (yCountNew = xCount - yCount = 8 - 0 = 8, xCount = 0).
        auto p = Rhythm::euclidean(8, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (8));
        for (int i = 0; i < 8; ++i) {
            TEST_ASSERT_TRUE(p[i]);
        }
    
}

void test_09_beats_steps_is_clamped_to_steps_euclidean_12_8_euclidean_8_8() {
        // Exercises the `beats = std::min(beats, steps)` guard.
        auto p = Rhythm::euclidean(12, 8);
        TEST_ASSERT_TRUE((int(p.size())) == (8));
        TEST_ASSERT_TRUE((countBeats(p)) == (8));
        for (int i = 0; i < 8; ++i) {
            TEST_ASSERT_TRUE(p[i]);
        }
    
}

void test_10_euclidean_1_1_single_step_pattern_has_exactly_one_beat() {
        auto p = Rhythm::euclidean(1, 1);
        TEST_ASSERT_TRUE((int(p.size())) == (1));
        TEST_ASSERT_TRUE((countBeats(p)) == (1));
        TEST_ASSERT_TRUE(p[0]);
    
}

// ---- structural properties -------------------------------------------------------

void test_11_pattern_size_equals_steps_for_a_range_of_inputs() {
        struct Row { int beats; int steps; };
        const Row table[] = {
            {1,  1},
            {3,  8},
            {5, 13},
            {7, 16},
            {4,  4},
            {0, 12},
            {12,12},
        };
        for (const auto &r : table) {
            auto p = Rhythm::euclidean(r.beats, r.steps);
            TEST_ASSERT_TRUE((int(p.size())) == (r.steps));
        }
    
}

void test_12_beat_count_equals_min_beats_steps_for_a_range_of_inputs() {
        struct Row { int beats; int steps; int expectedBeats; };
        const Row table[] = {
            {3,  8,  3},
            {5,  8,  5},
            {4, 16,  4},
            {0,  8,  0},
            {8,  8,  8},
            {12, 8,  8},   // clamped
            {1,  8,  1},
            {7,  8,  7},
        };
        for (const auto &r : table) {
            auto p = Rhythm::euclidean(r.beats, r.steps);
            TEST_ASSERT_TRUE((countBeats(p)) == (r.expectedBeats));
        }
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_euclidean_3_8_returns_classic_tresillo_1_0_0_1_0_0_1_0);
    RUN_TEST(test_02_euclidean_5_8_returns_1_0_1_1_0_1_1_0);
    RUN_TEST(test_03_euclidean_4_16_returns_four_evenly_spaced_beats);
    RUN_TEST(test_04_euclidean_7_8_returns_1_0_1_1_1_1_1_1);
    RUN_TEST(test_05_euclidean_1_8_returns_1_0_0_0_0_0_0_0);
    RUN_TEST(test_06_euclidean_5_13_multi_step_mixed_path);
    RUN_TEST(test_07_euclidean_0_8_returns_all_silent_pattern_of_size_8);
    RUN_TEST(test_08_euclidean_8_8_returns_all_active_pattern_of_size_8);
    RUN_TEST(test_09_beats_steps_is_clamped_to_steps_euclidean_12_8_euclidean_8_8);
    RUN_TEST(test_10_euclidean_1_1_single_step_pattern_has_exactly_one_beat);
    RUN_TEST(test_11_pattern_size_equals_steps_for_a_range_of_inputs);
    RUN_TEST(test_12_beat_count_equals_min_beats_steps_for_a_range_of_inputs);
    return UNITY_END();
}


