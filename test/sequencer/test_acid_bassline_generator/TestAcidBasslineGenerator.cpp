/**
 * @file TestAcidBasslineGenerator.cpp
 * @brief Implements the TestAcidBasslineGenerator component used by the unit test.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include <unity.h>

#include "Config.h"

#ifdef CONFIG_ACID_BASS_GENERATOR

#include "engine/generators/AcidBasslineGenerator.h"
#include "engine/generators/SequenceBuilder.h"
#include "model/NoteSequence.h"

#include "core/utils/StringBuilder.h"

void test_01_mode_returns_acidbassline() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        TEST_ASSERT_TRUE((gen.mode()) == (Generator::Mode::AcidBassline));
    
}

void test_02_paramcount_equals_the_number_of_param_enum_entries() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        TEST_ASSERT_TRUE((gen.paramCount()) == (int(AcidBasslineGenerator::Param::Last)));
    
}

void test_03_paramname_returns_non_null_for_every_param_and_nullptr_for_last() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        TEST_ASSERT_TRUE(gen.paramName(int(AcidBasslineGenerator::Param::Seed))          != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(AcidBasslineGenerator::Param::RootNote))       != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(AcidBasslineGenerator::Param::PatternLength))  != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(AcidBasslineGenerator::Param::Density))        != nullptr);
        TEST_ASSERT_TRUE(gen.paramName(int(AcidBasslineGenerator::Param::LegatoMix))      != nullptr);
        TEST_ASSERT_FALSE(gen.paramName(int(AcidBasslineGenerator::Param::Last))          != nullptr);
    
}

void test_04_same_seed_produces_identical_patterns_determinism() {
        NoteSequence seqA, seqB;
        seqA.clear(); seqB.clear();
        AcidBasslineGenerator::Params paramsA, paramsB;
        paramsA.seed = 42;
        paramsB.seed = 42;
        NoteSequenceBuilder builderA(seqA, NoteSequence::Layer::Note);
        NoteSequenceBuilder builderB(seqB, NoteSequence::Layer::Note);
        AcidBasslineGenerator genA(builderA, paramsA);
        AcidBasslineGenerator genB(builderB, paramsB);

        for (size_t i = 0; i < genA.pattern().size(); ++i) {
            TEST_ASSERT_TRUE((int(genA.pattern()[i])) == (int(genB.pattern()[i])));
        }
    
}

void test_05_different_seeds_produce_different_patterns() {
        NoteSequence seqA, seqB;
        seqA.clear(); seqB.clear();
        AcidBasslineGenerator::Params paramsA, paramsB;
        paramsA.seed = 1;
        paramsB.seed = 2;
        NoteSequenceBuilder builderA(seqA, NoteSequence::Layer::Note);
        NoteSequenceBuilder builderB(seqB, NoteSequence::Layer::Note);
        AcidBasslineGenerator genA(builderA, paramsA);
        AcidBasslineGenerator genB(builderB, paramsB);

        bool anyDiff = false;
        for (size_t i = 0; i < genA.pattern().size(); ++i) {
            if (genA.pattern()[i] != genB.pattern()[i]) {
                anyDiff = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anyDiff);
    
}

void test_06_setseed_clamps_to_0_65535() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.setSeed(-1);
        TEST_ASSERT_TRUE((gen.seed()) == (0));

        gen.setSeed(65536);
        TEST_ASSERT_TRUE((gen.seed()) == (65535));

        gen.setSeed(1000);
        TEST_ASSERT_TRUE((gen.seed()) == (1000));
    
}

void test_07_setrootnote_clamps_to_0_11() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.setRootNote(-1);
        TEST_ASSERT_TRUE((gen.rootNote()) == (0));

        gen.setRootNote(12);
        TEST_ASSERT_TRUE((gen.rootNote()) == (11));

        gen.setRootNote(5);
        TEST_ASSERT_TRUE((gen.rootNote()) == (5));
    
}

void test_08_setpatternlength_clamps_to_1_config_step_count() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.setPatternLength(0);
        TEST_ASSERT_TRUE((gen.patternLength()) == (1));

        gen.setPatternLength(CONFIG_STEP_COUNT + 1);
        TEST_ASSERT_TRUE((gen.patternLength()) == (CONFIG_STEP_COUNT));

        gen.setPatternLength(32);
        TEST_ASSERT_TRUE((gen.patternLength()) == (32));
    
}

void test_09_setdensity_clamps_to_0_100() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.setDensity(-1);
        TEST_ASSERT_TRUE((gen.density()) == (0));

        gen.setDensity(101);
        TEST_ASSERT_TRUE((gen.density()) == (100));

        gen.setDensity(50);
        TEST_ASSERT_TRUE((gen.density()) == (50));
    
}

void test_10_setlegatomix_clamps_to_0_100() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.setLegatoMix(-1);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (0));

        gen.setLegatoMix(101);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (100));

        gen.setLegatoMix(70);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (70));
    
}

void test_11_editparam_without_shift_increments_all_params_by_value() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 100;
        params.rootNote      = 5;
        params.patternLength = 16;
        params.density       = 50;
        params.legatoMix     = 50;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::Seed), 10, false);
        TEST_ASSERT_TRUE((gen.seed()) == (110));

        gen.editParam(int(AcidBasslineGenerator::Param::RootNote), 3, false);
        TEST_ASSERT_TRUE((gen.rootNote()) == (8));

        gen.editParam(int(AcidBasslineGenerator::Param::PatternLength), 4, false);
        TEST_ASSERT_TRUE((gen.patternLength()) == (20));

        gen.editParam(int(AcidBasslineGenerator::Param::Density), 5, false);
        TEST_ASSERT_TRUE((gen.density()) == (55));

        gen.editParam(int(AcidBasslineGenerator::Param::LegatoMix), -10, false);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (40));
    
}

void test_12_editparam_param_last_is_a_no_op() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 42;
        params.rootNote      = 3;
        params.patternLength = 16;
        params.density       = 50;
        params.legatoMix     = 35;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::Last), 1, false);

        TEST_ASSERT_TRUE((gen.seed()) == (42));
        TEST_ASSERT_TRUE((gen.rootNote()) == (3));
        TEST_ASSERT_TRUE((gen.patternLength()) == (16));
        TEST_ASSERT_TRUE((gen.density()) == (50));
        TEST_ASSERT_TRUE((gen.legatoMix()) == (35));
    
}

void test_13_editparam_shift_true_on_seed_produces_a_value_in_0_65535() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed = 0;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::Seed), 1, true);

        TEST_ASSERT_TRUE(gen.seed() >= 0);
        TEST_ASSERT_TRUE(gen.seed() <= 65535);
    
}

void test_14_editparam_shift_true_on_patternlength_snaps_to_next_prev_multiple_of_16() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::PatternLength), 1, true);
        TEST_ASSERT_TRUE((gen.patternLength()) == (32));

        gen.editParam(int(AcidBasslineGenerator::Param::PatternLength), -1, true);
        TEST_ASSERT_TRUE((gen.patternLength()) == (16));
    
}

void test_15_editparam_shift_true_on_density_snaps_to_next_prev_multiple_of_25() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.density = 50;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::Density), 1, true);
        TEST_ASSERT_TRUE((gen.density()) == (75));

        gen.editParam(int(AcidBasslineGenerator::Param::Density), -1, true);
        TEST_ASSERT_TRUE((gen.density()) == (50));
    
}

void test_16_editparam_shift_true_on_legatomix_snaps_to_next_prev_multiple_of_25() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.legatoMix = 25;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::LegatoMix), 1, true);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (50));

        gen.editParam(int(AcidBasslineGenerator::Param::LegatoMix), -1, true);
        TEST_ASSERT_TRUE((gen.legatoMix()) == (25));
    
}

void test_17_printparam_formats_seed_and_patternlength_as_integers_density_and_legatomix_as_percentages() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 12345;
        params.rootNote      = 0;
        params.patternLength = 32;
        params.density       = 75;
        params.legatoMix     = 50;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        FixedStringBuilder<64> seed, length, density, legato;
        gen.printParam(int(AcidBasslineGenerator::Param::Seed),          seed);
        gen.printParam(int(AcidBasslineGenerator::Param::PatternLength), length);
        gen.printParam(int(AcidBasslineGenerator::Param::Density),       density);
        gen.printParam(int(AcidBasslineGenerator::Param::LegatoMix),     legato);

        TEST_ASSERT_EQUAL_STRING("12345", (const char *)seed);
        TEST_ASSERT_EQUAL_STRING("32", (const char *)length);
        TEST_ASSERT_EQUAL_STRING("75%", (const char *)density);
        TEST_ASSERT_EQUAL_STRING("50%", (const char *)legato);
    
}

void test_18_printparam_param_last_leaves_the_string_unchanged() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        FixedStringBuilder<32> untouched("unchanged");
        gen.printParam(int(AcidBasslineGenerator::Param::Last), untouched);
        TEST_ASSERT_EQUAL_STRING("unchanged", (const char *)untouched);
    
}

void test_19_init_resets_all_parameters_to_their_default_values() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 9999;
        params.rootNote      = 11;
        params.patternLength = 48;
        params.density       = 10;
        params.legatoMix     = 90;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.init();

        TEST_ASSERT_TRUE((gen.seed()) == (0));
        TEST_ASSERT_TRUE((gen.rootNote()) == (0));
        TEST_ASSERT_TRUE((gen.patternLength()) == (16));
        TEST_ASSERT_TRUE((gen.density()) == (62));
        TEST_ASSERT_TRUE((gen.legatoMix()) == (35));
    
}

void test_20_density_100_gates_every_step_in_the_pattern() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 7;
        params.density       = 100;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        AcidBasslineGenerator gen(builder, params);

        for (int i = 0; i < 16; ++i) {
            TEST_ASSERT_TRUE(sequence.step(i).gate());
        }
    
}

void test_21_step_0_is_always_gated_regardless_of_seed_or_density() {
        for (int seed = 0; seed < 8; ++seed) {
            NoteSequence sequence;
            sequence.clear();
            AcidBasslineGenerator::Params params;
            params.seed          = uint16_t(seed);
            params.density       = 62;
            params.patternLength = 16;
            NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
            AcidBasslineGenerator gen(builder, params);

            TEST_ASSERT_TRUE(sequence.step(0).gate());
        }
    
}

void test_22_patternlength_sets_firststep_0_and_laststep_length_1_in_the_sequence() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 55;
        params.patternLength = 24;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        AcidBasslineGenerator gen(builder, params);

        TEST_ASSERT_TRUE((sequence.firstStep()) == (0));
        TEST_ASSERT_TRUE((sequence.lastStep()) == (23));
    
}

void test_23_steps_beyond_patternlength_have_no_gate() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 123;
        params.density       = 100;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        AcidBasslineGenerator gen(builder, params);

        for (int i = 16; i < CONFIG_STEP_COUNT; ++i) {
            TEST_ASSERT_FALSE(sequence.step(i).gate());
        }
    
}

void test_24_different_rootnote_values_produce_different_note_outputs() {
        NoteSequence seqRoot0, seqRoot5;
        seqRoot0.clear(); seqRoot5.clear();

        AcidBasslineGenerator::Params params0;
        params0.seed          = 333;
        params0.rootNote      = 0;
        params0.density       = 100;
        params0.patternLength = 16;

        AcidBasslineGenerator::Params params5 = params0;
        params5.rootNote = 5;

        NoteSequenceBuilder builder0(seqRoot0, NoteSequence::Layer::Note);
        NoteSequenceBuilder builder5(seqRoot5, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen0(builder0, params0);
        AcidBasslineGenerator gen5(builder5, params5);

        bool anyDifference = false;
        for (int i = 0; i < 16; ++i) {
            if (seqRoot0.step(i).note() != seqRoot5.step(i).note()) {
                anyDifference = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anyDifference);
    
}

void test_25_update_after_seed_change_regenerates_a_different_pattern() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 10;
        params.density       = 100;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);
        GeneratorPattern before = gen.pattern();

        gen.setSeed(9999);
        gen.update();
        GeneratorPattern after = gen.pattern();

        bool anyDiff = false;
        for (size_t i = 0; i < before.size(); ++i) {
            if (before[i] != after[i]) {
                anyDiff = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anyDiff);
    
}

void test_26_note_values_written_to_the_sequence_are_within_notesequence_note_bounds() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 9876;
        params.density       = 100;
        params.patternLength = 32;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        for (int i = 0; i < 32; ++i) {
            const int note = sequence.step(i).note();
            TEST_ASSERT_TRUE(note >= NoteSequence::Note::Min);
            TEST_ASSERT_TRUE(note <= NoteSequence::Note::Max);
        }
    
}

void test_27_gateprobability_values_are_within_valid_range_for_gated_steps() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 111;
        params.density       = 100;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::GateProbability);
        AcidBasslineGenerator gen(builder, params);

        for (int i = 0; i < 16; ++i) {
            if (sequence.step(i).gate()) {
                const int gp = sequence.step(i).gateProbability();
                TEST_ASSERT_TRUE(gp >= NoteSequence::GateProbability::Min);
                TEST_ASSERT_TRUE(gp <= NoteSequence::GateProbability::Max);
            }
        }
    
}

void test_28_higher_legatomix_produces_greater_or_equal_total_step_length_than_legatomix_0() {
        NoteSequence seqLow, seqHigh;
        seqLow.clear(); seqHigh.clear();

        AcidBasslineGenerator::Params paramsLow;
        paramsLow.seed          = 77;
        paramsLow.density       = 100;
        paramsLow.patternLength = 32;
        paramsLow.legatoMix     = 0;

        AcidBasslineGenerator::Params paramsHigh = paramsLow;
        paramsHigh.legatoMix = 100;

        NoteSequenceBuilder builderLow(seqLow,   NoteSequence::Layer::Length);
        NoteSequenceBuilder builderHigh(seqHigh,  NoteSequence::Layer::Length);
        AcidBasslineGenerator genLow(builderLow,   paramsLow);
        AcidBasslineGenerator genHigh(builderHigh, paramsHigh);

        int totalLow = 0, totalHigh = 0;
        for (int i = 0; i < 32; ++i) {
            totalLow  += seqLow.step(i).length();
            totalHigh += seqHigh.step(i).length();
        }
        TEST_ASSERT_TRUE(totalHigh >= totalLow);
    
}

void test_29_printparam_formats_rootnote_as_note_name() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.rootNote = 0;  // C
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        FixedStringBuilder<32> rootNote;
        gen.printParam(int(AcidBasslineGenerator::Param::RootNote), rootNote);
        TEST_ASSERT_EQUAL_STRING("C", (const char *)rootNote);
    
}

void test_30_gate_offset_values_are_within_valid_bounds_for_gated_steps() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 77;
        params.density       = 100;
        params.patternLength = 32;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::GateOffset);
        AcidBasslineGenerator gen(builder, params);

        for (int i = 0; i < 32; ++i) {
            if (sequence.step(i).gate()) {
                const int offset = sequence.step(i).gateOffset();
                TEST_ASSERT_TRUE(offset >= 0);
                TEST_ASSERT_TRUE(offset <= NoteSequence::GateOffset::Max);
            }
        }
    
}

void test_31_legatomix_100_and_density_100_produce_at_least_one_slide_across_32_steps() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 100;
        params.density       = 100;
        params.legatoMix     = 100;
        params.patternLength = 32;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Slide);
        AcidBasslineGenerator gen(builder, params);

        bool anySlide = false;
        for (int i = 0; i < 32; ++i) {
            if (sequence.step(i).slide()) {
                anySlide = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anySlide);
    
}

void test_32_applyscale_uses_sequence_local_rootnote_when_non_negative() {
        AcidBasslineGenerator::Params params;
        params.seed          = 500;
        params.density       = 100;
        params.patternLength = 16;
        params.rootNote      = 0;  // generator root: C

        NoteSequence seqDefault, seqOverride;
        seqDefault.clear();
        seqOverride.clear();

        // seqDefault: sequence rootNote = -1 (use params.rootNote = 0)
        NoteSequenceBuilder builderDefault(seqDefault, NoteSequence::Layer::Note);
        AcidBasslineGenerator genDefault(builderDefault, params);

        // seqOverride: rootNote=5 on the sequence overrides params.rootNote
        seqOverride.setRootNote(5);
        NoteSequenceBuilder builderOverride(seqOverride, NoteSequence::Layer::Note);
        AcidBasslineGenerator genOverride(builderOverride, params);

        bool anyDifference = false;
        for (int i = 0; i < 16; ++i) {
            if (seqDefault.step(i).note() != seqOverride.step(i).note()) {
                anyDifference = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anyDifference);
    
}

void test_33_density_0_produces_fewer_gated_steps_than_density_100() {
        NoteSequence seqFull, seqSparse;
        seqFull.clear(); seqSparse.clear();

        AcidBasslineGenerator::Params paramsFull;
        paramsFull.seed          = 42;
        paramsFull.density       = 100;
        paramsFull.patternLength = 32;

        AcidBasslineGenerator::Params paramsSparse = paramsFull;
        paramsSparse.density = 0;

        NoteSequenceBuilder builderFull(seqFull,     NoteSequence::Layer::Gate);
        NoteSequenceBuilder builderSparse(seqSparse,  NoteSequence::Layer::Gate);
        AcidBasslineGenerator genFull(builderFull,     paramsFull);
        AcidBasslineGenerator genSparse(builderSparse, paramsSparse);

        int countFull = 0, countSparse = 0;
        for (int i = 0; i < 32; ++i) {
            if (seqFull.step(i).gate())   ++countFull;
            if (seqSparse.step(i).gate()) ++countSparse;
        }
        TEST_ASSERT_TRUE(countSparse < countFull);
    
}

void test_34_editparam_shift_true_on_seed_with_negative_value_also_stays_within_0_65535() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed = 0;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Note);
        AcidBasslineGenerator gen(builder, params);

        gen.editParam(int(AcidBasslineGenerator::Param::Seed), -1, true);

        TEST_ASSERT_TRUE(gen.seed() >= 0);
        TEST_ASSERT_TRUE(gen.seed() <= 65535);
    
}

void test_35_gate_layer_pattern_encodes_gated_steps_as_255_and_tail_as_0() {
        NoteSequence sequence;
        sequence.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 7;
        params.density       = 100;
        params.patternLength = 16;
        NoteSequenceBuilder builder(sequence, NoteSequence::Layer::Gate);
        AcidBasslineGenerator gen(builder, params);

        // All 16 steps gated → pattern must be 255
        for (int i = 0; i < 16; ++i) {
            TEST_ASSERT_TRUE((int(gen.pattern()[i])) == (255));
        }
        // Tail beyond patternLength must be 0
        for (int i = 16; i < CONFIG_STEP_COUNT; ++i) {
            TEST_ASSERT_TRUE((int(gen.pattern()[i])) == (0));
        }
    
}

void test_36_fallback_path_curvesequencebuilder_fills_pattern_via_note_layer_renderpreview() {
        CurveSequence curveSeq;
        curveSeq.clear();
        AcidBasslineGenerator::Params params;
        params.seed          = 42;
        params.density       = 100;
        params.patternLength = 16;
        // CurveSequenceBuilder is not a NoteSequenceBuilder → triggers fallback path
        CurveSequenceBuilder builder(curveSeq, CurveSequence::Layer::Gate);
        AcidBasslineGenerator gen(builder, params);

        // The fallback renders the Note layer into _pattern; at least step 0
        // (root note, always gated) must produce a non-zero encoded value.
        bool anyNonZero = false;
        for (int i = 0; i < 16; ++i) {
            if (gen.pattern()[i] != 0) {
                anyNonZero = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(anyNonZero);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_mode_returns_acidbassline);
    RUN_TEST(test_02_paramcount_equals_the_number_of_param_enum_entries);
    RUN_TEST(test_03_paramname_returns_non_null_for_every_param_and_nullptr_for_last);
    RUN_TEST(test_04_same_seed_produces_identical_patterns_determinism);
    RUN_TEST(test_05_different_seeds_produce_different_patterns);
    RUN_TEST(test_06_setseed_clamps_to_0_65535);
    RUN_TEST(test_07_setrootnote_clamps_to_0_11);
    RUN_TEST(test_08_setpatternlength_clamps_to_1_config_step_count);
    RUN_TEST(test_09_setdensity_clamps_to_0_100);
    RUN_TEST(test_10_setlegatomix_clamps_to_0_100);
    RUN_TEST(test_11_editparam_without_shift_increments_all_params_by_value);
    RUN_TEST(test_12_editparam_param_last_is_a_no_op);
    RUN_TEST(test_13_editparam_shift_true_on_seed_produces_a_value_in_0_65535);
    RUN_TEST(test_14_editparam_shift_true_on_patternlength_snaps_to_next_prev_multiple_of_16);
    RUN_TEST(test_15_editparam_shift_true_on_density_snaps_to_next_prev_multiple_of_25);
    RUN_TEST(test_16_editparam_shift_true_on_legatomix_snaps_to_next_prev_multiple_of_25);
    RUN_TEST(test_17_printparam_formats_seed_and_patternlength_as_integers_density_and_legatomix_as_percentages);
    RUN_TEST(test_18_printparam_param_last_leaves_the_string_unchanged);
    RUN_TEST(test_19_init_resets_all_parameters_to_their_default_values);
    RUN_TEST(test_20_density_100_gates_every_step_in_the_pattern);
    RUN_TEST(test_21_step_0_is_always_gated_regardless_of_seed_or_density);
    RUN_TEST(test_22_patternlength_sets_firststep_0_and_laststep_length_1_in_the_sequence);
    RUN_TEST(test_23_steps_beyond_patternlength_have_no_gate);
    RUN_TEST(test_24_different_rootnote_values_produce_different_note_outputs);
    RUN_TEST(test_25_update_after_seed_change_regenerates_a_different_pattern);
    RUN_TEST(test_26_note_values_written_to_the_sequence_are_within_notesequence_note_bounds);
    RUN_TEST(test_27_gateprobability_values_are_within_valid_range_for_gated_steps);
    RUN_TEST(test_28_higher_legatomix_produces_greater_or_equal_total_step_length_than_legatomix_0);
    RUN_TEST(test_29_printparam_formats_rootnote_as_note_name);
    RUN_TEST(test_30_gate_offset_values_are_within_valid_bounds_for_gated_steps);
    RUN_TEST(test_31_legatomix_100_and_density_100_produce_at_least_one_slide_across_32_steps);
    RUN_TEST(test_32_applyscale_uses_sequence_local_rootnote_when_non_negative);
    RUN_TEST(test_33_density_0_produces_fewer_gated_steps_than_density_100);
    RUN_TEST(test_34_editparam_shift_true_on_seed_with_negative_value_also_stays_within_0_65535);
    RUN_TEST(test_35_gate_layer_pattern_encodes_gated_steps_as_255_and_tail_as_0);
    RUN_TEST(test_36_fallback_path_curvesequencebuilder_fills_pattern_via_note_layer_renderpreview);
    return UNITY_END();
}


#endif // CONFIG_ACID_BASS_GENERATOR

