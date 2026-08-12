/**
 * @file TestCurve.cpp
 * @brief Unity regression tests for sequencer curve evaluation.
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

#include "model/Curve.h"

#include <cmath>

namespace {

constexpr float Epsilon = 0.0001f;

void assertNear(float expected, float actual) {
    TEST_ASSERT_FLOAT_WITHIN(Epsilon, expected, actual);
}

} // namespace

void setUp() {}
void tearDown() {}

void test_function_table_matches_eval_for_every_curve_type() {
    for (int type = 0; type < Curve::Last; ++type) {
        const Curve::Type curve = static_cast<Curve::Type>(type);
        TEST_ASSERT_NOT_NULL(Curve::function(curve));
        assertNear(Curve::function(curve)(0.37f), Curve::eval(curve, 0.37f));
    }
}

void test_constant_step_and_ramp_curves_have_expected_values() {
    assertNear(0.f, Curve::eval(Curve::Low, 0.f));
    assertNear(0.f, Curve::eval(Curve::Low, 1.f));
    assertNear(1.f, Curve::eval(Curve::High, 0.f));
    assertNear(1.f, Curve::eval(Curve::High, 1.f));

    assertNear(0.f, Curve::eval(Curve::StepUp, 0.499f));
    assertNear(1.f, Curve::eval(Curve::StepUp, 0.5f));
    assertNear(1.f, Curve::eval(Curve::StepDown, 0.499f));
    assertNear(0.f, Curve::eval(Curve::StepDown, 0.5f));

    assertNear(0.25f, Curve::eval(Curve::RampUp, 0.25f));
    assertNear(0.75f, Curve::eval(Curve::RampDown, 0.25f));
}

void test_exponential_logarithmic_and_smooth_curves_match_reference_points() {
    assertNear(0.25f, Curve::eval(Curve::ExpUp, 0.5f));
    assertNear(0.25f, Curve::eval(Curve::ExpDown, 0.5f));
    assertNear(std::sqrt(0.25f), Curve::eval(Curve::LogUp, 0.25f));
    assertNear(std::sqrt(0.75f), Curve::eval(Curve::LogDown, 0.25f));
    assertNear(0.5f, Curve::eval(Curve::SmoothUp, 0.5f));
    assertNear(0.5f, Curve::eval(Curve::SmoothDown, 0.5f));
}

void test_triangle_and_bell_curves_have_expected_symmetry_and_peaks() {
    assertNear(0.f, Curve::eval(Curve::Triangle, 0.f));
    assertNear(1.f, Curve::eval(Curve::Triangle, 0.5f));
    assertNear(0.f, Curve::eval(Curve::Triangle, 1.f));
    assertNear(Curve::eval(Curve::Triangle, 0.25f), Curve::eval(Curve::Triangle, 0.75f));

    assertNear(0.f, Curve::eval(Curve::Bell, 0.f));
    assertNear(1.f, Curve::eval(Curve::Bell, 0.5f));
    assertNear(0.f, Curve::eval(Curve::Bell, 1.f));
    assertNear(Curve::eval(Curve::Bell, 0.25f), Curve::eval(Curve::Bell, 0.75f));
}

void test_repeating_exponential_curves_reset_at_subdivision_boundaries() {
    assertNear(1.f, Curve::eval(Curve::ExpDown2x, 0.f));
    assertNear(1.f, Curve::eval(Curve::ExpDown2x, 0.5f));
    assertNear(0.f, Curve::eval(Curve::ExpDown2x, 1.f));

    assertNear(1.f, Curve::eval(Curve::ExpDown3x, 0.f));
    assertNear(1.f, Curve::eval(Curve::ExpDown3x, 1.f / 3.f));
    assertNear(0.f, Curve::eval(Curve::ExpDown3x, 1.f));

    assertNear(1.f, Curve::eval(Curve::ExpDown4x, 0.f));
    assertNear(1.f, Curve::eval(Curve::ExpDown4x, 0.25f));
    assertNear(0.f, Curve::eval(Curve::ExpDown4x, 1.f));
}

void test_all_curve_outputs_stay_normalized_on_unit_interval() {
    for (int type = 0; type < Curve::Last; ++type) {
        const Curve::Type curve = static_cast<Curve::Type>(type);
        for (int sample = 0; sample <= 100; ++sample) {
            const float x = static_cast<float>(sample) / 100.f;
            const float value = Curve::eval(curve, x);
            TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(-Epsilon, value);
            TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.f + Epsilon, value);
        }
    }
}

void test_monotonic_curve_families_move_in_expected_direction() {
    const Curve::Type increasing[] = {
        Curve::RampUp, Curve::ExpUp, Curve::LogUp, Curve::SmoothUp,
    };
    const Curve::Type decreasing[] = {
        Curve::RampDown, Curve::ExpDown, Curve::LogDown, Curve::SmoothDown,
    };

    for (Curve::Type type : increasing) {
        float previous = Curve::eval(type, 0.f);
        for (int sample = 1; sample <= 100; ++sample) {
            const float value = Curve::eval(type, static_cast<float>(sample) / 100.f);
            TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(previous - Epsilon, value);
            previous = value;
        }
    }

    for (Curve::Type type : decreasing) {
        float previous = Curve::eval(type, 0.f);
        for (int sample = 1; sample <= 100; ++sample) {
            const float value = Curve::eval(type, static_cast<float>(sample) / 100.f);
            TEST_ASSERT_LESS_OR_EQUAL_FLOAT(previous + Epsilon, value);
            previous = value;
        }
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_function_table_matches_eval_for_every_curve_type);
    RUN_TEST(test_constant_step_and_ramp_curves_have_expected_values);
    RUN_TEST(test_exponential_logarithmic_and_smooth_curves_match_reference_points);
    RUN_TEST(test_triangle_and_bell_curves_have_expected_symmetry_and_peaks);
    RUN_TEST(test_repeating_exponential_curves_reset_at_subdivision_boundaries);
    RUN_TEST(test_all_curve_outputs_stay_normalized_on_unit_interval);
    RUN_TEST(test_monotonic_curve_families_move_in_expected_direction);
    return UNITY_END();
}
