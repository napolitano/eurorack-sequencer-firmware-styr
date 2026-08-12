/**
 * @file TestClipBoard.cpp
 * @brief Implements the TestClipBoard component used by the unit test.
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

#include "model/ClipBoard.h"
#include "model/Project.h"

void test_01_track_starts_in_a_defined_default_state() {
        Track track;

        TEST_ASSERT_TRUE((int(track.trackMode())) == (int(Track::TrackMode::Default)));
        TEST_ASSERT_TRUE((track.linkTrack()) == (-1));
    
}

void test_02_clipboard_safely_replaces_stored_object_types() {
        Project project;
        project.clear();
        ClipBoard clipBoard(project);

        auto &noteSource = project.noteSequence(0, 0);
        auto &noteDestination = project.noteSequence(1, 0);
        noteSource.setLastStep(3);
        noteDestination.setLastStep(7);

        clipBoard.copyNoteSequence(noteSource);
        clipBoard.pasteNoteSequence(noteDestination);
        TEST_ASSERT_TRUE((noteDestination.lastStep()) == (3));

        project.setTrackMode(0, Track::TrackMode::Curve);
        auto &curveSource = project.track(0).curveTrack().sequence(0);
        project.setTrackMode(1, Track::TrackMode::Curve);
        auto &curveDestination = project.track(1).curveTrack().sequence(0);
        curveSource.setLastStep(5);
        curveDestination.setLastStep(9);

        clipBoard.copyCurveSequence(curveSource);
        clipBoard.pasteCurveSequence(curveDestination);
        TEST_ASSERT_TRUE((curveDestination.lastStep()) == (5));

        clipBoard.clear();
        TEST_ASSERT_FALSE(clipBoard.canPasteCurveSequence());
    
}

void test_03_pattern_clipboard_preserves_mixed_note_and_curve_sequence_storage() {
        Project project;
        project.clear();
        ClipBoard clipBoard(project);

        project.setTrackMode(0, Track::TrackMode::Note);
        project.setTrackMode(1, Track::TrackMode::Curve);
        project.setTrackMode(2, Track::TrackMode::Lfo);

        project.track(0).noteTrack().sequence(0).setLastStep(2);
        project.track(1).curveTrack().sequence(0).setLastStep(4);

        clipBoard.copyPattern(0);

        project.track(0).noteTrack().sequence(0).setLastStep(8);
        project.track(1).curveTrack().sequence(0).setLastStep(10);
        clipBoard.pastePattern(0);

        TEST_ASSERT_TRUE((project.track(0).noteTrack().sequence(0).lastStep()) == (2));
        TEST_ASSERT_TRUE((project.track(1).curveTrack().sequence(0).lastStep()) == (4));
        TEST_ASSERT_TRUE((int(project.track(2).trackMode())) == (int(Track::TrackMode::Lfo)));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_track_starts_in_a_defined_default_state);
    RUN_TEST(test_02_clipboard_safely_replaces_stored_object_types);
    RUN_TEST(test_03_pattern_clipboard_preserves_mixed_note_and_curve_sequence_storage);
    return UNITY_END();
}

