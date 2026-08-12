/**
 * @file TestMidiLearn.cpp
 * @brief Implements the TestMidiLearn component used by the unit test.
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

#include "engine/MidiLearn.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
namespace {

// Build a raw pitch-bend message (status 0xE0 | channel, data 0, 0).
// MidiMessage::makePitchBend() has a known clamping quirk but we only need
// a message whose isPitchBend() returns true; the data bytes do not matter.
static MidiMessage makePitchBend(int channel) {
    return MidiMessage(uint8_t(0xe0 | channel), 0, 0);
}

// Send `count` identical pitch-bend messages.
static void sendPitchBends(MidiLearn &ml, MidiPort port, int channel, int count) {
    for (int i = 0; i < count; ++i) {
        ml.receiveMidi(port, makePitchBend(channel));
    }
}

// Send `count` identical CC messages.
static void sendCCs(MidiLearn &ml, MidiPort port, int channel, int cc, int value, int count) {
    for (int i = 0; i < count; ++i) {
        ml.receiveMidi(port, MidiMessage::makeControlChange(uint8_t(channel), uint8_t(cc), uint8_t(value)));
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

// ── Lifecycle: start / stop / isActive ──────────────────────────────────

void test_01_isactive_reflects_start_and_stop_state() {
        MidiLearn ml;
        TEST_ASSERT_FALSE(ml.isActive());

        bool called = false;
        ml.start([&](const MidiLearn::Result &) { called = true; });
        TEST_ASSERT_TRUE(ml.isActive());

        ml.stop();
        TEST_ASSERT_FALSE(ml.isActive());
        (void)called;
    
}

void test_02_receivemidi_is_a_no_op_when_no_callback_is_set() {
        MidiLearn ml;
        // Normally two NoteOn on the same note would fire a callback,
        // but no start() was called so nothing should happen.
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_FALSE(ml.isActive());
    
}

void test_03_stop_clears_the_callback_so_further_messages_produce_no_result() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });
        ml.stop();

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_TRUE((callCount) == (0));
    
}

// ── Note detection ───────────────────────────────────────────────────────

void test_04_two_noteon_messages_with_the_same_note_emit_a_note_result() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_TRUE((callCount) == (0));   // only 1 event → no emit yet

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.port) == (MidiPort::Midi));
        TEST_ASSERT_TRUE((int(result.channel)) == (0));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::Note));
        TEST_ASSERT_TRUE((int(result.note)) == (60));
    
}

void test_05_noteoff_counts_toward_the_note_counter_just_like_noteon() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 64));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOff(0, 64));

        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::Note));
        TEST_ASSERT_TRUE((int(result.note)) == (64));
    
}

void test_06_note_change_resets_the_note_counter_and_requires_two_more_messages_on_the_new_note() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60)); // counter = 1 (note 60)
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 62)); // note change → counter reset to 1 (note 62)
        TEST_ASSERT_TRUE((callCount) == (0));  // threshold not yet reached

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 62)); // counter = 2 → emit
        TEST_ASSERT_TRUE((callCount) == (1));
    
}

void test_07_identical_learned_result_is_emitted_again_while_session_stays_active() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_TRUE((callCount) == (1));

        // Current behavior: same learned mapping can emit again.
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60));
        TEST_ASSERT_TRUE((callCount) == (2));
    
}

void test_08_start_begins_a_fresh_learn_session_so_same_mapping_can_be_emitted_again() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61));
        TEST_ASSERT_TRUE((callCount) == (1));

        ml.stop();
        ml.start([&](const MidiLearn::Result &) { ++callCount; });
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61));
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61));
        TEST_ASSERT_TRUE((callCount) == (2));
    
}

// ── PitchBend detection ──────────────────────────────────────────────────

void test_09_eight_pitchbend_messages_emit_a_pitchbend_result() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        sendPitchBends(ml, MidiPort::UsbMidi, 3, 7);
        TEST_ASSERT_TRUE((callCount) == (0));  // 7 → still one short

        sendPitchBends(ml, MidiPort::UsbMidi, 3, 1);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.port) == (MidiPort::UsbMidi));
        TEST_ASSERT_TRUE((int(result.channel)) == (3));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::PitchBend));
    
}

// ── ControlAbsolute detection ────────────────────────────────────────────

void test_10_eight_cc_messages_with_absolute_value_emit_a_controlabsolute_result() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // value=64: not in (0,8) and not in (64,72) → absolute
        sendCCs(ml, MidiPort::Midi, 0, 74, 64, 7);
        TEST_ASSERT_TRUE((callCount) == (0));

        sendCCs(ml, MidiPort::Midi, 0, 74, 64, 1);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlAbsolute));
        TEST_ASSERT_TRUE((int(result.controlNumber)) == (74));
    
}

void test_11_cc_value_0_is_treated_as_absolute_boundary_not_0() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        sendCCs(ml, MidiPort::Midi, 0, 10, 0, 8);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlAbsolute));
    
}

void test_12_cc_value_8_is_treated_as_absolute_boundary_not_8() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        sendCCs(ml, MidiPort::Midi, 0, 10, 8, 8);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlAbsolute));
    
}

// ── ControlRelative detection ────────────────────────────────────────────

void test_13_eight_cc_messages_with_value_1_7_emit_a_controlrelative_result() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // value=3: in (0,8) → relative
        sendCCs(ml, MidiPort::Midi, 0, 20, 3, 8);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlRelative));
        TEST_ASSERT_TRUE((int(result.controlNumber)) == (20));
    
}

void test_14_eight_cc_messages_with_value_65_71_emit_a_controlrelative_result() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // value=66: in (64,72) → relative
        sendCCs(ml, MidiPort::Midi, 0, 20, 66, 8);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlRelative));
        TEST_ASSERT_TRUE((int(result.controlNumber)) == (20));
    
}

void test_15_result_metadata_channel_and_cc_number_are_forwarded_correctly() {
        MidiLearn ml;
        MidiLearn::Result absResult{}, relResult{};

        ml.start([&](const MidiLearn::Result &r) {
            if (r.event == MidiLearn::Event::ControlAbsolute) absResult = r;
            if (r.event == MidiLearn::Event::ControlRelative) relResult = r;
        });

        sendCCs(ml, MidiPort::Midi, 2, 74, 100, 8); // absolute, ch 2
        TEST_ASSERT_TRUE((int(absResult.channel)) == (2));
        TEST_ASSERT_TRUE((int(absResult.controlNumber)) == (74));

        // Restart for relative detection
        ml.stop();
        ml.start([&](const MidiLearn::Result &r) {
            if (r.event == MidiLearn::Event::ControlRelative) relResult = r;
        });

        sendCCs(ml, MidiPort::Midi, 1, 20, 5, 8);  // relative, ch 1
        TEST_ASSERT_TRUE((int(relResult.channel)) == (1));
        TEST_ASSERT_TRUE((int(relResult.controlNumber)) == (20));
    
}

// ── NRPN filter ──────────────────────────────────────────────────────────

void test_16_nrpn_pair_cc_0_31_then_cc_32_63_is_filtered_only_the_first_cc_counts() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // 8 pairs of CC#10 (absolute) + CC#42 (NRPN companion, filtered).
        // CC#10 increments the ControlAbsolute counter on each iteration.
        // CC#42 is silently discarded by the NRPN guard.
        for (int i = 0; i < 8; ++i) {
            ml.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 10, 64)); // CC#10: not NRPN
            ml.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 42, 64)); // CC#42: NRPN → filtered
        }

        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlAbsolute));
        TEST_ASSERT_TRUE((int(result.controlNumber)) == (10));
    
}

void test_17_nrpn_guard_requires_preceding_cc_to_be_in_0_31_cc_32_63_pair_is_not_filtered() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // CC#50 (not in 0-31) followed by CC#42 → NOT a NRPN pair; both count normally.
        // Both are absolute (value=64). They alternate → each change resets counters.
        // 8 alternating messages never reach 8 on either one.
        for (int i = 0; i < 4; ++i) {
            ml.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 50, 64));
            ml.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 42, 64));
        }
        TEST_ASSERT_TRUE((callCount) == (0)); // counters kept resetting due to CC number alternation

        // Now send 8 identical CC#50 to confirm detection still works.
        sendCCs(ml, MidiPort::Midi, 0, 50, 64, 8);
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.event) == (MidiLearn::Event::ControlAbsolute));
        TEST_ASSERT_TRUE((int(result.controlNumber)) == (50));
    
}

// ── Port and channel reset ────────────────────────────────────────────────

void test_18_port_change_resets_state_and_detection_restarts_on_the_new_port() {
        MidiLearn ml;
        MidiLearn::Result result{};
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &r) { result = r; ++callCount; });

        // Begin accumulating on Midi port.
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60)); // Midi, counter=1

        // Switch to UsbMidi → reset, counter starts from scratch.
        ml.receiveMidi(MidiPort::UsbMidi, MidiMessage::makeNoteOn(0, 60)); // reset + counter=1
        TEST_ASSERT_TRUE((callCount) == (0));  // not yet at 2

        ml.receiveMidi(MidiPort::UsbMidi, MidiMessage::makeNoteOn(0, 60)); // counter=2 → emit
        TEST_ASSERT_TRUE((callCount) == (1));
        TEST_ASSERT_TRUE((result.port) == (MidiPort::UsbMidi));
    
}

void test_19_channel_change_on_the_same_port_resets_state() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60)); // ch 0, counter=1
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(1, 60)); // ch 1 → reset, counter=1
        TEST_ASSERT_TRUE((callCount) == (0));

        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(1, 60)); // ch 1, counter=2 → emit
        TEST_ASSERT_TRUE((callCount) == (1));
    
}

// ── CC number change resets counters ─────────────────────────────────────

void test_20_cc_number_change_resets_both_absolute_and_relative_counters() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });

        sendCCs(ml, MidiPort::Midi, 0, 74, 64, 7);  // absolute counter = 7
        // Change CC number → counters reset to 0
        ml.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 75, 64));
        TEST_ASSERT_TRUE((callCount) == (0));  // threshold not reached

        sendCCs(ml, MidiPort::Midi, 0, 75, 64, 7);  // 1 + 7 = 8 on CC#75 → emit
        TEST_ASSERT_TRUE((callCount) == (1));
    
}

// ── Unrecognized message types ────────────────────────────────────────────

void test_21_programchange_messages_do_not_increment_any_counter() {
        MidiLearn ml;
        int callCount = 0;

        ml.start([&](const MidiLearn::Result &) { ++callCount; });

        // ProgramChange has no matching branch in receiveMidi → early return.
        for (int i = 0; i < 100; ++i) {
            ml.receiveMidi(MidiPort::Midi, MidiMessage::makeProgramChange(0, uint8_t(i & 0x7f)));
        }
        TEST_ASSERT_TRUE((callCount) == (0));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_isactive_reflects_start_and_stop_state);
    RUN_TEST(test_02_receivemidi_is_a_no_op_when_no_callback_is_set);
    RUN_TEST(test_03_stop_clears_the_callback_so_further_messages_produce_no_result);
    RUN_TEST(test_04_two_noteon_messages_with_the_same_note_emit_a_note_result);
    RUN_TEST(test_05_noteoff_counts_toward_the_note_counter_just_like_noteon);
    RUN_TEST(test_06_note_change_resets_the_note_counter_and_requires_two_more_messages_on_the_new_note);
    RUN_TEST(test_07_identical_learned_result_is_emitted_again_while_session_stays_active);
    RUN_TEST(test_08_start_begins_a_fresh_learn_session_so_same_mapping_can_be_emitted_again);
    RUN_TEST(test_09_eight_pitchbend_messages_emit_a_pitchbend_result);
    RUN_TEST(test_10_eight_cc_messages_with_absolute_value_emit_a_controlabsolute_result);
    RUN_TEST(test_11_cc_value_0_is_treated_as_absolute_boundary_not_0);
    RUN_TEST(test_12_cc_value_8_is_treated_as_absolute_boundary_not_8);
    RUN_TEST(test_13_eight_cc_messages_with_value_1_7_emit_a_controlrelative_result);
    RUN_TEST(test_14_eight_cc_messages_with_value_65_71_emit_a_controlrelative_result);
    RUN_TEST(test_15_result_metadata_channel_and_cc_number_are_forwarded_correctly);
    RUN_TEST(test_16_nrpn_pair_cc_0_31_then_cc_32_63_is_filtered_only_the_first_cc_counts);
    RUN_TEST(test_17_nrpn_guard_requires_preceding_cc_to_be_in_0_31_cc_32_63_pair_is_not_filtered);
    RUN_TEST(test_18_port_change_resets_state_and_detection_restarts_on_the_new_port);
    RUN_TEST(test_19_channel_change_on_the_same_port_resets_state);
    RUN_TEST(test_20_cc_number_change_resets_both_absolute_and_relative_counters);
    RUN_TEST(test_21_programchange_messages_do_not_increment_any_counter);
    return UNITY_END();
}


