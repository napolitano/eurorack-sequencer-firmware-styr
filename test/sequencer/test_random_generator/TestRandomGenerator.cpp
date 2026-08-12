/**
 * @file TestRandomGenerator.cpp
 * @brief Implements the TestRandomGenerator component used by the unit test.
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

#include "engine/generators/RandomGenerator.h"

#include "model/NoteSequence.h"

#include "core/utils/StringBuilder.h"

namespace {

static bool patternsEqual(const GeneratorPattern &a, const GeneratorPattern &b) {
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

static bool hasAnyDifference(const GeneratorPattern &a, const GeneratorPattern &b) {
    return !patternsEqual(a, b);
}

} // namespace

void test_01_constructor_update_is_deterministic_for_identical_params() {
        NoteSequence sequenceA;
        sequenceA.clear();
        RandomGenerator::Params paramsA;
        paramsA.seed = 123;
        paramsA.smooth = 0;
        paramsA.bias = 0;
        paramsA.scale = 10;
        NoteSequenceBuilder builderA(sequenceA, NoteSequence::Layer::Note);
        RandomGenerator genA(builderA, paramsA);

        NoteSequence sequenceB;
        sequenceB.clear();
        RandomGenerator::Params paramsB;
        paramsB.seed = 123;
        paramsB.smooth = 0;
        paramsB.bias = 0;
        paramsB.scale = 10;
        NoteSequenceBuilder builderB(sequenceB, NoteSequence::Layer::Note);
        RandomGenerator genB(builderB, paramsB);

        TEST_ASSERT_TRUE(patternsEqual(genA.pattern(), genB.pattern()));
    
}

void test_02_paramname_editparam_and_printparam_cover_clamping_and_formatting() {
        NoteSequence sequence;
        sequence.clear();
        RandomGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        RandomGenerator gen(builder, params);

        TEST_ASSERT_TRUE(gen.paramName(int(RandomGenerator::Param::Seed)) != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(RandomGenerator::Param::Smooth)) != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(RandomGenerator::Param::Bias)) != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(RandomGenerator::Param::Scale)) != nullptr);
        TEST_ASSERT_FALSE(gen.paramName(int(RandomGenerator::Param::Last)) != nullptr);

        gen.editParam(int(RandomGenerator::Param::Seed), 5000, false);
        gen.editParam(int(RandomGenerator::Param::Smooth), 100, false);
        gen.editParam(int(RandomGenerator::Param::Bias), -100, false);
        gen.editParam(int(RandomGenerator::Param::Scale), -100, false);

        TEST_ASSERT_TRUE((gen.seed()) == (1000));
        TEST_ASSERT_TRUE((gen.smooth()) == (10));
        TEST_ASSERT_TRUE((gen.bias()) == (-10));
        TEST_ASSERT_TRUE((gen.scale()) == (0));

        FixedStringBuilder<32> seed;
        FixedStringBuilder<32> smooth;
        FixedStringBuilder<32> bias;
        FixedStringBuilder<32> scale;

        gen.printParam(int(RandomGenerator::Param::Seed), seed);
        gen.printParam(int(RandomGenerator::Param::Smooth), smooth);
        gen.printParam(int(RandomGenerator::Param::Bias), bias);
        gen.printParam(int(RandomGenerator::Param::Scale), scale);

        TEST_ASSERT_EQUAL_STRING("1000", (const char *)seed);
        TEST_ASSERT_EQUAL_STRING("10", (const char *)smooth);
        TEST_ASSERT_EQUAL_STRING("-10", (const char *)bias);
        TEST_ASSERT_EQUAL_STRING("0", (const char *)scale);

        const int seedBefore = gen.seed();
        const int smoothBefore = gen.smooth();
        const int biasBefore = gen.bias();
        const int scaleBefore = gen.scale();

        gen.editParam(int(RandomGenerator::Param::Last), 123, false);

        TEST_ASSERT_TRUE((gen.seed()) == (seedBefore));
        TEST_ASSERT_TRUE((gen.smooth()) == (smoothBefore));
        TEST_ASSERT_TRUE((gen.bias()) == (biasBefore));
        TEST_ASSERT_TRUE((gen.scale()) == (scaleBefore));

        FixedStringBuilder<32> untouched("unchanged");
        gen.printParam(int(RandomGenerator::Param::Last), untouched);
        TEST_ASSERT_EQUAL_STRING("unchanged", (const char *)untouched);
    
}

void test_03_init_restores_default_params_and_recomputes_the_pattern() {
        NoteSequence sequence;
        sequence.clear();
        RandomGenerator::Params params;
        params.seed = 777;
        params.smooth = 6;
        params.bias = 5;
        params.scale = 30;

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        RandomGenerator gen(builder, params);

        gen.init();

        TEST_ASSERT_TRUE((gen.seed()) == (0));
        TEST_ASSERT_TRUE((gen.smooth()) == (0));
        TEST_ASSERT_TRUE((gen.bias()) == (0));
        TEST_ASSERT_TRUE((gen.scale()) == (10));
    
}

void test_04_update_applies_smoothing_and_scale_transformations() {
        NoteSequence sequence;
        sequence.clear();
        RandomGenerator::Params params;
        params.seed = 345;
        params.smooth = 0;
        params.bias = 0;
        params.scale = 10;

        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        RandomGenerator gen(builder, params);
        GeneratorPattern basePattern = gen.pattern();

        gen.setSmooth(10);
        gen.update();
        GeneratorPattern smoothPattern = gen.pattern();
        TEST_ASSERT_TRUE(hasAnyDifference(basePattern, smoothPattern));

        gen.setSmooth(0);
        gen.setScale(0);
        gen.setBias(0);
        gen.update();

        for (size_t i = 0; i < gen.pattern().size(); ++i) {
            TEST_ASSERT_TRUE((int(gen.pattern()[i])) == (127));
        }

        gen.setScale(100);
        gen.setBias(10);
        gen.update();

        for (size_t i = 0; i < gen.pattern().size(); ++i) {
            TEST_ASSERT_TRUE(gen.pattern()[i] <= 255);
        }
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_constructor_update_is_deterministic_for_identical_params);
    RUN_TEST(test_02_paramname_editparam_and_printparam_cover_clamping_and_formatting);
    RUN_TEST(test_03_init_restores_default_params_and_recomputes_the_pattern);
    RUN_TEST(test_04_update_applies_smoothing_and_scale_transformations);
    return UNITY_END();
}


