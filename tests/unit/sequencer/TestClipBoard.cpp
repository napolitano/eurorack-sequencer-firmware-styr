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
#include "UnitTest.h"

#include "model/ClipBoard.h"
#include "model/Project.h"

UNIT_TEST("ClipBoard") {

    CASE("Track starts in a defined default state") {
        Track track;

        expectEqual(int(track.trackMode()), int(Track::TrackMode::Default));
        expectEqual(track.linkTrack(), -1);
    }

    CASE("clipboard safely replaces stored object types") {
        Project project;
        project.clear();
        ClipBoard clipBoard(project);

        auto &noteSource = project.noteSequence(0, 0);
        auto &noteDestination = project.noteSequence(1, 0);
        noteSource.setLastStep(3);
        noteDestination.setLastStep(7);

        clipBoard.copyNoteSequence(noteSource);
        clipBoard.pasteNoteSequence(noteDestination);
        expectEqual(noteDestination.lastStep(), 3);

        project.setTrackMode(0, Track::TrackMode::Curve);
        auto &curveSource = project.track(0).curveTrack().sequence(0);
        project.setTrackMode(1, Track::TrackMode::Curve);
        auto &curveDestination = project.track(1).curveTrack().sequence(0);
        curveSource.setLastStep(5);
        curveDestination.setLastStep(9);

        clipBoard.copyCurveSequence(curveSource);
        clipBoard.pasteCurveSequence(curveDestination);
        expectEqual(curveDestination.lastStep(), 5);

        clipBoard.clear();
        expectFalse(clipBoard.canPasteCurveSequence());
    }

    CASE("pattern clipboard preserves mixed Note and Curve sequence storage") {
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

        expectEqual(project.track(0).noteTrack().sequence(0).lastStep(), 2);
        expectEqual(project.track(1).curveTrack().sequence(0).lastStep(), 4);
        expectEqual(int(project.track(2).trackMode()), int(Track::TrackMode::Lfo));
    }
}
