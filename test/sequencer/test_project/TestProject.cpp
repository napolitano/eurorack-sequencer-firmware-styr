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
#include <unity.h>

#include "MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"
#include "core/hash/FnvHash.h"

#include "model/Project.h"
#include "model/ProjectVersion.h"

void test_01_clear_sets_sensible_defaults_and_notifies() {
        Project p;
        std::vector<Project::Event> events;
        p.watch([&events] (Project::Event e) { events.push_back(e); });

        p.clear();
        // clear should notify ProjectCleared (may be preceded by selected-track/pattern notifications)
        TEST_ASSERT_TRUE(!events.empty());
        bool foundCleared = false;
        for (auto e : events) if (e == Project::ProjectCleared) { foundCleared = true; break; }
        TEST_ASSERT_TRUE(foundCleared);

        // default tempo and swing
        TEST_ASSERT_TRUE(std::abs(p.tempo() - 120.f) < 1e-3f);
        TEST_ASSERT_TRUE((int(p.swing())) == (50));

        // default selected indices
        TEST_ASSERT_TRUE((int(p.selectedTrackIndex())) == (0));
        TEST_ASSERT_TRUE((int(p.selectedPatternIndex())) == (0));
    
}

void test_02_selected_track_pattern_notifications_and_bounds() {
        Project p;
        std::vector<Project::Event> events;
        p.watch([&events] (Project::Event e) { events.push_back(e); });

        // change selected track index -> should produce SelectedTrackIndexChanged and SelectedPatternIndexChanged
        p.setSelectedTrackIndex(2);
        TEST_ASSERT_TRUE(events.size() >= 2);
        TEST_ASSERT_TRUE((int(events[0])) == (int(Project::SelectedTrackIndexChanged)));
        TEST_ASSERT_TRUE((int(events[1])) == (int(Project::SelectedPatternIndexChanged)));

        // out-of-bounds selection clamps
        p.setSelectedTrackIndex(999);
        TEST_ASSERT_TRUE(p.selectedTrackIndex() <= CONFIG_TRACK_COUNT - 1);
        p.setSelectedPatternIndex(999);
        TEST_ASSERT_TRUE(p.selectedPatternIndex() <= CONFIG_PATTERN_COUNT - 1);
    
}

void test_03_clearpattern_resets_per_pattern_data() {
        Project p;
        // set a non-default last step on track 0 pattern 0
        p.noteSequence(0, 0).setLastStep(7);
        TEST_ASSERT_TRUE((int(p.noteSequence(0,0).lastStep())) == (7));
        p.clearPattern(0);
        // after clearPattern, last step should be reset to default (CONFIG_STEP_COUNT-1)
        TEST_ASSERT_TRUE((int(p.noteSequence(0,0).lastStep())) == (15));
    
}

void test_04_write_read_roundtrip_preserves_selected_fields() {
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
        TEST_ASSERT_TRUE(ok);

        TEST_ASSERT_TRUE((std::string(dst.name())) == (std::string(src.name())));
        TEST_ASSERT_TRUE(std::abs(dst.tempo() - src.tempo()) < 1e-3f);
        TEST_ASSERT_TRUE((int(dst.swing())) == (int(src.swing())));
        TEST_ASSERT_TRUE((int(dst.selectedTrackIndex())) == (int(src.selectedTrackIndex())));
        TEST_ASSERT_TRUE((int(dst.selectedPatternIndex())) == (int(src.selectedPatternIndex())));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_clear_sets_sensible_defaults_and_notifies);
    RUN_TEST(test_02_selected_track_pattern_notifications_and_bounds);
    RUN_TEST(test_03_clearpattern_resets_per_pattern_data);
    RUN_TEST(test_04_write_read_roundtrip_preserves_selected_fields);
    return UNITY_END();
}

