/**
 * @file TestEuclideanGenerator.cpp
 * @brief Implements the TestEuclideanGenerator component used by the unit test.
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

#include "engine/generators/EuclideanGenerator.h"

#include "model/NoteSequence.h"

#include "core/utils/StringBuilder.h"

namespace {

static bool patternEquals(const Rhythm::Pattern &a, const Rhythm::Pattern &b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

} // namespace

void test_01_constructor_update_builds_expected_shifted_euclidean_pattern_and_sets_sequence_length() {
        NoteSequence sequence;
        sequence.clear();
        EuclideanGenerator::Params params;
        params.steps = 13;
        params.beats = 5;
        params.offset = 3;

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        EuclideanGenerator generator(builder, params);

        const auto expected = Rhythm::euclidean(params.beats, params.steps).shifted(params.offset);

        TEST_ASSERT_TRUE(patternEquals(generator.pattern(), expected));
        TEST_ASSERT_TRUE((builder.length()) == (int(params.steps)));

        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            const float expectedValue = expected[i % expected.size()] ? 1.f : 0.f;
            TEST_ASSERT_TRUE((builder.value(i)) == (expectedValue));
        }
    
}

void test_02_paramname_editparam_and_printparam_cover_clamping_formatting_and_param_last_no_op() {
        NoteSequence sequence;
        sequence.clear();
        EuclideanGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        EuclideanGenerator generator(builder, params);

        TEST_ASSERT_TRUE(generator.paramName(int(EuclideanGenerator::Param::Steps)) != nullptr);
        TEST_ASSERT_TRUE(generator.paramName(int(EuclideanGenerator::Param::Beats)) != nullptr);
        TEST_ASSERT_TRUE(generator.paramName(int(EuclideanGenerator::Param::Offset)) != nullptr);
        TEST_ASSERT_FALSE(generator.paramName(int(EuclideanGenerator::Param::Last)) != nullptr);

        generator.editParam(int(EuclideanGenerator::Param::Steps), 1000, false);
        generator.editParam(int(EuclideanGenerator::Param::Beats), 1000, false);
        generator.editParam(int(EuclideanGenerator::Param::Offset), 1000, false);

        TEST_ASSERT_TRUE((generator.steps()) == (CONFIG_STEP_COUNT));
        TEST_ASSERT_TRUE((generator.beats()) == (CONFIG_STEP_COUNT));
        TEST_ASSERT_TRUE((generator.offset()) == (CONFIG_STEP_COUNT - 1));

        FixedStringBuilder<32> steps;
        FixedStringBuilder<32> beats;
        FixedStringBuilder<32> offset;

        generator.printParam(int(EuclideanGenerator::Param::Steps), steps);
        generator.printParam(int(EuclideanGenerator::Param::Beats), beats);
        generator.printParam(int(EuclideanGenerator::Param::Offset), offset);

        TEST_ASSERT_EQUAL_STRING("64", (const char *)steps);
        TEST_ASSERT_EQUAL_STRING("64", (const char *)beats);
        TEST_ASSERT_EQUAL_STRING("63", (const char *)offset);

        const int stepsBefore = generator.steps();
        const int beatsBefore = generator.beats();
        const int offsetBefore = generator.offset();

        generator.editParam(int(EuclideanGenerator::Param::Last), 123, false);

        TEST_ASSERT_TRUE((generator.steps()) == (stepsBefore));
        TEST_ASSERT_TRUE((generator.beats()) == (beatsBefore));
        TEST_ASSERT_TRUE((generator.offset()) == (offsetBefore));

        FixedStringBuilder<32> untouched("unchanged");
        generator.printParam(int(EuclideanGenerator::Param::Last), untouched);
        TEST_ASSERT_EQUAL_STRING("unchanged", (const char *)untouched);
    
}

void test_03_init_restores_default_params_and_recomputes_pattern() {
        NoteSequence sequence;
        sequence.clear();
        EuclideanGenerator::Params params;
        params.steps = 31;
        params.beats = 17;
        params.offset = 9;

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        EuclideanGenerator generator(builder, params);

        generator.init();

        TEST_ASSERT_TRUE((generator.steps()) == (16));
        TEST_ASSERT_TRUE((generator.beats()) == (4));
        TEST_ASSERT_TRUE((generator.offset()) == (0));
        TEST_ASSERT_TRUE((builder.length()) == (16));

        const auto expected = Rhythm::euclidean(4, 16).shifted(0);
        TEST_ASSERT_TRUE(patternEquals(generator.pattern(), expected));
    
}

void test_04_update_handles_beats_larger_than_steps_by_delegating_clamped_rhythm_generation() {
        NoteSequence sequence;
        sequence.clear();
        EuclideanGenerator::Params params;
        params.steps = 4;
        params.beats = 64;
        params.offset = 1;

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        EuclideanGenerator generator(builder, params);

        const auto expected = Rhythm::euclidean(params.beats, params.steps).shifted(params.offset);
        TEST_ASSERT_TRUE(patternEquals(generator.pattern(), expected));

        for (int i = 0; i < 12; ++i) {
            TEST_ASSERT_TRUE((builder.value(i)) == (expected[i % expected.size()] ? 1.f : 0.f));
        }
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_constructor_update_builds_expected_shifted_euclidean_pattern_and_sets_sequence_length);
    RUN_TEST(test_02_paramname_editparam_and_printparam_cover_clamping_formatting_and_param_last_no_op);
    RUN_TEST(test_03_init_restores_default_params_and_recomputes_pattern);
    RUN_TEST(test_04_update_handles_beats_larger_than_steps_by_delegating_clamped_rhythm_generation);
    return UNITY_END();
}



