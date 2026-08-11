/**
 * @file TestProject.cpp
 * @brief Implements the TestProject component used by the unit test.
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

#include "../core/io/MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"
#include "core/hash/FnvHash.h"

#include "model/Project.h"
#include "model/ProjectVersion.h"

UNIT_TEST("Project: basic behavior and serialization") {
    CASE("clear sets sensible defaults and notifies") {
        Project p;
        std::vector<Project::Event> events;
        p.watch([&events] (Project::Event e) { events.push_back(e); });

        p.clear();
        // clear should notify ProjectCleared (may be preceded by selected-track/pattern notifications)
        expectTrue(!events.empty());
        bool foundCleared = false;
        for (auto e : events) if (e == Project::ProjectCleared) { foundCleared = true; break; }
        expectTrue(foundCleared);

        // default tempo and swing
        expectTrue(std::abs(p.tempo() - 120.f) < 1e-3f);
        expectEqual(int(p.swing()), 50);

        // default selected indices
        expectEqual(int(p.selectedTrackIndex()), 0);
        expectEqual(int(p.selectedPatternIndex()), 0);
    }

    CASE("selected track/pattern notifications and bounds") {
        Project p;
        std::vector<Project::Event> events;
        p.watch([&events] (Project::Event e) { events.push_back(e); });

        // change selected track index -> should produce SelectedTrackIndexChanged and SelectedPatternIndexChanged
        p.setSelectedTrackIndex(2);
        expectTrue(events.size() >= 2);
        expectEqual(int(events[0]), int(Project::SelectedTrackIndexChanged));
        expectEqual(int(events[1]), int(Project::SelectedPatternIndexChanged));

        // out-of-bounds selection clamps
        p.setSelectedTrackIndex(999);
        expectTrue(p.selectedTrackIndex() <= CONFIG_TRACK_COUNT - 1);
        p.setSelectedPatternIndex(999);
        expectTrue(p.selectedPatternIndex() <= CONFIG_PATTERN_COUNT - 1);
    }

    CASE("clearPattern resets per-pattern data") {
        Project p;
        // set a non-default last step on track 0 pattern 0
        p.noteSequence(0, 0).setLastStep(7);
        expectEqual(int(p.noteSequence(0,0).lastStep()), 7);
        p.clearPattern(0);
        // after clearPattern, last step should be reset to default (CONFIG_STEP_COUNT-1)
        expectEqual(int(p.noteSequence(0,0).lastStep()), 15);
    }

    CASE("write/read roundtrip preserves selected fields") {
        // prepare source project
        Project src;
        src.clear();
        src.setName("ProjTest");
        src.setTempo(123.5f);
        src.setSwing(60);
        src.setSelectedTrackIndex(3);
        src.setSelectedPatternIndex(2);

        MemoryWriter memoryWriter(nullptr, 0);
        VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, ProjectVersion::Version32);
        src.write(writer);

        const auto &outBuf = memoryWriter.buffer();
        MemoryReader memoryReader(outBuf.data(), outBuf.size());
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Version32);

        Project dst;
        dst.clear();
        bool ok = dst.read(reader);
        expectTrue(ok);

        expectEqual(std::string(dst.name()), std::string(src.name()));
        expectTrue(std::abs(dst.tempo() - src.tempo()) < 1e-3f);
        expectEqual(int(dst.swing()), int(src.swing()));
        expectEqual(int(dst.selectedTrackIndex()), int(src.selectedTrackIndex()));
        expectEqual(int(dst.selectedPatternIndex()), int(src.selectedPatternIndex()));
    }
}
