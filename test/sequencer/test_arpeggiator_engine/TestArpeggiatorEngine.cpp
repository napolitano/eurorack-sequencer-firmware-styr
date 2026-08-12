/**
 * @file TestArpeggiatorEngine.cpp
 * @brief Implements the TestArpeggiatorEngine component used by the unit test.
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

#include "engine/ArpeggiatorEngine.h"

#include <initializer_list>
#include <memory>
#include <vector>

namespace {

struct Harness {
    Arpeggiator arpeggiator;
    std::unique_ptr<ArpeggiatorEngine> engine;

    Harness() {
        arpeggiator.clear();
        arpeggiator.setDivisor(1);
        engine.reset(new ArpeggiatorEngine(arpeggiator));
    }
};

static std::vector<ArpeggiatorEngine::Event> collectEvents(ArpeggiatorEngine &engine, uint32_t lastTick, int swing = 50) {
    std::vector<ArpeggiatorEngine::Event> events;
    for (uint32_t tick = 0; tick <= lastTick; ++tick) {
        engine.tick(tick, swing);
        ArpeggiatorEngine::Event event;
        while (engine.getEvent(tick, event)) {
            events.push_back(event);
        }
    }
    return events;
}

static std::vector<int> collectNoteOns(ArpeggiatorEngine &engine, uint32_t lastTick, int swing = 50) {
    std::vector<int> notes;
    for (const auto &event : collectEvents(engine, lastTick, swing)) {
        if (event.action == ArpeggiatorEngine::Event::NoteOn) {
            notes.push_back(event.note);
        }
    }
    return notes;
}

static void expectNotes(const std::vector<int> &actual, std::initializer_list<int> expected) {
    TEST_ASSERT_TRUE((int(actual.size())) == (int(expected.size())));
    int index = 0;
    for (int note : expected) {
        TEST_ASSERT_TRUE((actual[index]) == (note));
        ++index;
    }
}

static void expectAllNotesInSet(const std::vector<int> &actual, std::initializer_list<int> expected) {
    for (int note : actual) {
        bool found = false;
        for (int expectedNote : expected) {
            if (note == expectedNote) {
                found = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(found);
    }
}

} // namespace

void test_01_empty_tick_emits_no_events_and_note_off_without_hold_removes_notes() {
        Harness harness;
        ArpeggiatorEngine::Event event;

        harness.engine->tick(0, 50);
        TEST_ASSERT_FALSE(harness.engine->getEvent(0, event));

        harness.engine->noteOn(60);
        harness.engine->noteOff(60);
        harness.engine->tick(0, 50);
        TEST_ASSERT_FALSE(harness.engine->getEvent(0, event));
    
}

void test_02_play_order_keeps_insertion_order_ignores_duplicates_caps_note_count_and_delays_legato_note_off_by_one_tick() {
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::PlayOrder);
            harness.arpeggiator.setGateLength(100);

            harness.engine->noteOn(64);
            harness.engine->noteOn(60);
            harness.engine->noteOn(67);
            harness.engine->noteOn(64);
            harness.engine->noteOn(72);
            harness.engine->noteOn(74);
            harness.engine->noteOn(76);
            harness.engine->noteOn(77);
            harness.engine->noteOn(79);
            harness.engine->noteOn(81);

            ArpeggiatorEngine::Event event;
            harness.engine->tick(0, 50);
            TEST_ASSERT_TRUE(harness.engine->getEvent(0, event));
            TEST_ASSERT_TRUE((int(event.action)) == (int(ArpeggiatorEngine::Event::NoteOn)));
            TEST_ASSERT_TRUE((int(event.note)) == (64));
            TEST_ASSERT_FALSE(harness.engine->getEvent(0, event));
        }

        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::PlayOrder);
            harness.arpeggiator.setGateLength(100);

            harness.engine->noteOn(64);
            harness.engine->noteOn(60);
            harness.engine->noteOn(67);
            harness.engine->noteOn(64);
            harness.engine->noteOn(72);
            harness.engine->noteOn(74);
            harness.engine->noteOn(76);
            harness.engine->noteOn(77);
            harness.engine->noteOn(79);
            harness.engine->noteOn(81);

            const auto events = collectEvents(*harness.engine, 31);

            std::vector<int> notes;
            int firstNoteOffTick = -1;
            for (const auto &current : events) {
                if (current.action == ArpeggiatorEngine::Event::NoteOn) {
                    notes.push_back(current.note);
                } else if (firstNoteOffTick == -1) {
                    firstNoteOffTick = int(current.tick);
                }
            }

            expectNotes(notes, { 64, 60, 67, 72, 74, 76, 77, 79 });
            TEST_ASSERT_TRUE((firstNoteOffTick) == (5));
        }
    
}

void test_03_hold_mode_keeps_released_notes_until_all_holds_are_gone_and_a_new_note_resets_stale_state() {
        {
            Harness harness;
            harness.arpeggiator.setHold(true);
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);

            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOff(60);

            const auto notes = collectNoteOns(*harness.engine, 4);
            expectNotes(notes, { 60, 64 });
        }

        {
            Harness harness;
            harness.arpeggiator.setHold(true);
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);

            harness.engine->noteOn(60);
            harness.engine->noteOff(60);
            harness.engine->noteOn(67);

            const auto notes = collectNoteOns(*harness.engine, 0);
            expectNotes(notes, { 67 });
        }
    
}

void test_04_note_off_handles_missing_notes_repeated_hold_releases_and_middle_note_removal() {
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);

            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOff(61);

            const auto notes = collectNoteOns(*harness.engine, 8);
            expectNotes(notes, { 60, 64, 60 });
        }

        {
            Harness harness;
            harness.arpeggiator.setHold(true);
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);

            harness.engine->noteOn(60);
            harness.engine->noteOff(60);
            harness.engine->noteOff(60);

            const auto notes = collectNoteOns(*harness.engine, 8);
            expectNotes(notes, { 60, 60, 60 });
        }

        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);

            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            harness.engine->noteOff(64);

            const auto notes = collectNoteOns(*harness.engine, 8);
            expectNotes(notes, { 60, 67, 60 });
        }
    
}

void test_05_directional_and_center_based_modes_emit_the_expected_note_order() {
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 12), { 60, 64, 67, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Down);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 12), { 67, 64, 60, 67 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::UpDown);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 12), { 60, 64, 67, 64 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::DownUp);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 12), { 67, 64, 60, 64 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::UpDown);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 8), { 60, 60, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::UpAndDown);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 20), { 60, 64, 67, 67, 64, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::DownAndUp);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 20), { 67, 64, 60, 60, 64, 67 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Converge);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 8), { 60, 67, 64 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Diverge);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            expectNotes(collectNoteOns(*harness.engine, 8), { 64, 60, 67 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Random);
            harness.engine->noteOn(60);
            harness.engine->noteOn(64);
            harness.engine->noteOn(67);
            const auto notes = collectNoteOns(*harness.engine, 16);
            TEST_ASSERT_TRUE((int(notes.size())) == (5));
            expectAllNotesInSet(notes, { 60, 64, 67 });
        }
    
}

void test_06_octave_handling_covers_zero_positive_negative_and_both_direction_ranges() {
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.arpeggiator.setOctaves(0);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 8), { 60, 60, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.arpeggiator.setOctaves(1);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 8), { 60, 72, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.arpeggiator.setOctaves(-1);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 8), { 60, 48, 60 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.arpeggiator.setOctaves(7);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 28), { 60, 72, 84, 84, 72, 60, 60, 72 });
        }
        {
            Harness harness;
            harness.arpeggiator.setMode(Arpeggiator::Mode::Up);
            harness.arpeggiator.setOctaves(-7);
            harness.engine->noteOn(60);
            expectNotes(collectNoteOns(*harness.engine, 28), { 60, 48, 36, 36, 48, 60, 60, 48 });
        }
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_empty_tick_emits_no_events_and_note_off_without_hold_removes_notes);
    RUN_TEST(test_02_play_order_keeps_insertion_order_ignores_duplicates_caps_note_count_and_delays_legato_note_off_by_one_tick);
    RUN_TEST(test_03_hold_mode_keeps_released_notes_until_all_holds_are_gone_and_a_new_note_resets_stale_state);
    RUN_TEST(test_04_note_off_handles_missing_notes_repeated_hold_releases_and_middle_note_removal);
    RUN_TEST(test_05_directional_and_center_based_modes_emit_the_expected_note_order);
    RUN_TEST(test_06_octave_handling_covers_zero_positive_negative_and_both_direction_ranges);
    return UNITY_END();
}





