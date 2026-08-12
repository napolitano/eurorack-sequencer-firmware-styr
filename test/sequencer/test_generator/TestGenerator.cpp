/**
 * @file TestGenerator.cpp
 * @brief Implements the TestGenerator component used by the unit test.
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

#include "engine/generators/Generator.h"

#include "model/NoteSequence.h"

namespace {

static NoteSequence makeSequenceWithEditedLayer(NoteSequence::Layer layer) {
    NoteSequence sequence;
    sequence.clear();
    sequence.step(0).setLayerValue(layer, NoteSequence::layerRange(layer).max);
    sequence.step(1).setLayerValue(layer, NoteSequence::layerRange(layer).min);
    return sequence;
}

} // namespace

void test_01_initlayer_clears_the_selected_layer_and_returns_no_generator() {
        auto sequence = makeSequenceWithEditedLayer(NoteSequence::Layer::Note);
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);

        Generator *generator = Generator::execute(Generator::Mode::InitLayer, builder);
        TEST_ASSERT_FALSE(generator != nullptr);

        const int expected = NoteSequence::layerDefaultValue(NoteSequence::Layer::Note);
        TEST_ASSERT_TRUE((sequence.step(0).layerValue(NoteSequence::Layer::Note)) == (expected));
        TEST_ASSERT_TRUE((sequence.step(1).layerValue(NoteSequence::Layer::Note)) == (expected));
    
}

void test_02_euclidean_mode_returns_an_euclidean_generator_instance() {
        NoteSequence sequence;
        sequence.clear();
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        Generator *generator = Generator::execute(Generator::Mode::Euclidean, builder);
        TEST_ASSERT_TRUE(generator != nullptr);
        TEST_ASSERT_TRUE((generator->mode()) == (Generator::Mode::Euclidean));
    
}

void test_03_random_mode_returns_a_random_generator_instance() {
        NoteSequence sequence;
        sequence.clear();
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        Generator *generator = Generator::execute(Generator::Mode::Random, builder);
        TEST_ASSERT_TRUE(generator != nullptr);
        TEST_ASSERT_TRUE((generator->mode()) == (Generator::Mode::Random));
    
}

#ifdef CONFIG_ACID_BASS_GENERATOR

void test_04_acidbassline_mode_returns_an_acidbassline_generator_instance() {
        NoteSequence sequence;
        sequence.clear();
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        Generator *generator = Generator::execute(Generator::Mode::AcidBassline, builder);
        TEST_ASSERT_TRUE(generator != nullptr);
        TEST_ASSERT_TRUE((generator->mode()) == (Generator::Mode::AcidBassline));
    
}

#endif

void test_05_last_mode_falls_back_to_nullptr() {
        NoteSequence sequence;
        sequence.clear();
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);

        Generator *generator = Generator::execute(Generator::Mode::Last, builder);
        TEST_ASSERT_FALSE(generator != nullptr);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_initlayer_clears_the_selected_layer_and_returns_no_generator);
    RUN_TEST(test_02_euclidean_mode_returns_an_euclidean_generator_instance);
    RUN_TEST(test_03_random_mode_returns_a_random_generator_instance);
    RUN_TEST(test_04_acidbassline_mode_returns_an_acidbassline_generator_instance);
    RUN_TEST(test_05_last_mode_falls_back_to_nullptr);
    return UNITY_END();
}


