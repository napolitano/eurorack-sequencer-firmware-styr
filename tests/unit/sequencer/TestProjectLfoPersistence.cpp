/**
 * @file TestProjectLfoPersistence.cpp
 * @brief Implements the TestProjectLfoPersistence component used by the unit test.
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

UNIT_TEST("Project: LfoTrack end-to-end persistence") {
    CASE("project write/read roundtrip v32") {
        uint8_t buf[16384];
        std::memset(buf, 0, sizeof(buf));

        // prepare source project
        Project src;
        src.clear();
        src.setName("LFO Test");
        src.setSelectedTrackIndex(0);
        auto &t = src.track(0);
        src.setTrackMode(0, Track::TrackMode::Lfo);
        auto &l = src.track(0).lfoTrack();
        l.clear();
        l.setWaveform(LfoTrack::Waveform::RandomSoft);
        l.setMode(LfoTrack::Mode::Sync);
        l.setClip(true);
        l.setHi(250);
        l.setLow(-250);
        l.setSpeed(16);
        l.setFreeFreqCentihz(123);
        l.setPulseWidth(75);
        l.setNudge(5);

        // Use a dynamic-only MemoryWriter (no fixed out buffer) to avoid overflow
        // reporting from copying into a limited stack buffer; tests read from the
        // writer's dynamic buffer below.
        MemoryWriter memoryWriter(nullptr, 0);
        VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, ProjectVersion::Version32);
        src.write(writer);
        printf("Project bytesWritten=%zu\n", memoryWriter.bytesWritten()); fflush(stdout);
        // Ensure the test helper didn't overflow the provided fixed buffer
        expectTrue(!memoryWriter.overflowed());
        // Debug: print tail of serialized buffer (last up to 12 bytes) in hex
        const auto &dbgBuf = memoryWriter.buffer();
        size_t dbgLen = dbgBuf.size();
        size_t tail = dbgLen >= 12 ? 12 : dbgLen;
        if (tail) {
            printf("Project tail (last %zu bytes):", tail); fflush(stdout);
            for (size_t i = dbgLen - tail; i < dbgLen; ++i) printf(" %02x", dbgBuf[i]);
            printf("\n"); fflush(stdout);
            if (dbgLen >= 4) {
                // Print stored hash (last 4 bytes, little-endian)
                uint32_t stored = uint32_t(dbgBuf[dbgLen - 4]) | (uint32_t(dbgBuf[dbgLen - 3])<<8) | (uint32_t(dbgBuf[dbgLen - 2])<<16) | (uint32_t(dbgBuf[dbgLen - 1])<<24);
                printf("Project stored-hash=0x%08x\n", stored); fflush(stdout);
                if (dbgLen >= 8) {
                    FnvHash h;
                    // Writer hashes data after the 4-byte version and before the final 4-byte stored hash
                    h(dbgBuf.data() + 4, dbgLen - 4 - 4);
                    printf("Project computed-hash=0x%08x\n", h.result()); fflush(stdout);
                }
                if (dbgLen >= 4) {
                    uint32_t dataVer = uint32_t(dbgBuf[0]) | (uint32_t(dbgBuf[1])<<8) | (uint32_t(dbgBuf[2])<<16) | (uint32_t(dbgBuf[3])<<24);
                    printf("Project data-version=%u\n", dataVer); fflush(stdout);
                }
            }
        }

        // read into dst project
        // Use the writer's internal dynamic buffer for reading so tests don't fail when
        // serialized size exceeds the stack buffer capacity.
        const auto &outBuf = memoryWriter.buffer();
        MemoryReader memoryReader(outBuf.data(), outBuf.size());
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Version32);

        Project dst;
        dst.clear();
        bool ok = dst.read(reader);
        printf("Project bytesRead after read=%zu\n", memoryReader.bytesRead()); fflush(stdout);
        expectTrue(ok);

        auto &ld = dst.track(0).lfoTrack();
        expectEqual(int(ld.waveform()), int(l.waveform()));
        expectEqual(int(ld.mode()), int(l.mode()));
        expectEqual(ld.clip(), l.clip());
        expectEqual(ld.hi(), int(l.hi()));
        expectEqual(ld.low(), int(l.low()));
        expectEqual(ld.speed(), int(l.speed()));
        expectEqual(ld.freeFreqCentihz(), int(l.freeFreqCentihz()));
        expectEqual(ld.pulseWidth(), int(l.pulseWidth()));
        expectEqual(ld.nudge(), int(l.nudge()));
    }
}




