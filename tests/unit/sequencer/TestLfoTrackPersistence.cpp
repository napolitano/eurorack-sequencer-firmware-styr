/**
 * @file TestLfoTrackPersistence.cpp
 * @brief Implements the TestLfoTrackPersistence component used by the unit test.
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

#include "model/LfoTrack.h"
#include "model/ProjectVersion.h"

static uint8_t buf[1024];

static void clearBuf() {
    std::memset(buf, 0, sizeof(buf));
}

UNIT_TEST("LfoTrackPersistence") {
    CASE("persistence roundtrip v32") {
        clearBuf();

        LfoTrack src;
        src.clear();
        src.setWaveform(LfoTrack::Waveform::RandomSoft);
        src.setMode(LfoTrack::Mode::Sync);
        src.setClip(true);
        src.setHi(250);
        src.setLow(-250);
        src.setSpeed(16);
        src.setFreeFreqCentihz(123);
        src.setPulseWidth(75);
        src.setNudge(5);

        // Use a dynamic-only MemoryWriter (no fixed out buffer) to avoid overflow
        // reporting from copying into a limited stack buffer; tests read from the
        // writer's dynamic buffer below.
        MemoryWriter memoryWriter(nullptr, 0);
        VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, ProjectVersion::Version32);
        src.write(writer);
        writer.writeHash();
        expectTrue(!memoryWriter.overflowed());
        // Debug: print tail bytes and stored hash from the dynamic buffer
        const auto &dbgBuf = memoryWriter.buffer();
        printf("LfoTrack bytesWritten=%zu\n", dbgBuf.size()); fflush(stdout);
        if (dbgBuf.size() >= 1) {
            size_t tail = dbgBuf.size() >= 12 ? 12 : dbgBuf.size();
            printf("LfoTrack tail:"); for (size_t i = dbgBuf.size()-tail; i < dbgBuf.size(); ++i) printf(" %02x", dbgBuf[i]); printf("\n"); fflush(stdout);
            if (dbgBuf.size() >= 4) {
                uint32_t stored = uint32_t(dbgBuf[dbgBuf.size() - 4]) | (uint32_t(dbgBuf[dbgBuf.size() - 3])<<8) | (uint32_t(dbgBuf[dbgBuf.size() - 2])<<16) | (uint32_t(dbgBuf[dbgBuf.size() - 1])<<24);
                printf("LfoTrack stored-hash=0x%08x\n", stored); fflush(stdout);
                if (dbgBuf.size() >= 8) {
                    FnvHash h;
                    h(dbgBuf.data() + 4, dbgBuf.size() - 4 - 4);
                    printf("LfoTrack computed-hash=0x%08x\n", h.result()); fflush(stdout);
                }
                if (dbgBuf.size() >= 4) {
                    uint32_t dataVer = uint32_t(dbgBuf[0]) | (uint32_t(dbgBuf[1])<<8) | (uint32_t(dbgBuf[2])<<16) | (uint32_t(dbgBuf[3])<<24);
                    printf("LfoTrack data-version=%u\n", dataVer); fflush(stdout);
                }
            }
        }

        // Use the writer's internal dynamic buffer for reading so tests don't fail when
        // serialized size exceeds the fixed stack buffer capacity.
        const auto &outBuf = memoryWriter.buffer();
        MemoryReader memoryReader(outBuf.data(), outBuf.size());
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Version32);

        LfoTrack dst;
        dst.clear();
        dst.read(reader);

        expectEqual(int(dst.waveform()), int(src.waveform()));
        expectEqual(int(dst.mode()), int(src.mode()));
        expectEqual(dst.clip(), src.clip());
        expectEqual(dst.hi(), int(src.hi()));
        expectEqual(dst.low(), int(src.low()));
        expectEqual(dst.speed(), int(src.speed()));
        expectEqual(dst.freeFreqCentihz(), int(src.freeFreqCentihz()));
        expectEqual(dst.pulseWidth(), int(src.pulseWidth()));
        expectEqual(dst.nudge(), int(src.nudge()));
        expectTrue(reader.checkHash());
    }

    CASE("read older v31 stream keeps default nudge") {
        clearBuf();

        // Create a v31 stream (older format without nudge)
        MemoryWriter memoryWriter(nullptr, 0);
        VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, ProjectVersion::Version31);

        // Write fields in the old order (without nudge)
        LfoTrack::Waveform wf = LfoTrack::Waveform::Triangle;
        LfoTrack::Mode md = LfoTrack::Mode::Free;
        writer.writeEnum(wf, [](LfoTrack::Waveform w){ return static_cast<uint8_t>(w); });
        writer.writeEnum(md, [](LfoTrack::Mode m){ return static_cast<uint8_t>(m); });
        bool clip = false; writer.write(clip);
        int16_t hi = 100; writer.write(hi);
        int16_t low = -100; writer.write(low);
        int16_t speed = 32; writer.write(speed);
        int16_t freefreq = 200; writer.write(freefreq);
        int8_t pulseWidth = 60; writer.write(pulseWidth);

        writer.writeHash();
        expectTrue(!memoryWriter.overflowed());
        // Debug: show bytes written for the v31 stream
        const auto &dbgBuf2 = memoryWriter.buffer();
        printf("LfoTrack(v31) bytesWritten=%zu\n", dbgBuf2.size()); fflush(stdout);
        if (dbgBuf2.size() >= 4) {
            uint32_t stored = uint32_t(dbgBuf2[dbgBuf2.size() - 4]) | (uint32_t(dbgBuf2[dbgBuf2.size() - 3])<<8) | (uint32_t(dbgBuf2[dbgBuf2.size() - 2])<<16) | (uint32_t(dbgBuf2[dbgBuf2.size() - 1])<<24);
            printf("LfoTrack(v31) stored-hash=0x%08x\n", stored); fflush(stdout);
            if (dbgBuf2.size() >= 8) {
                FnvHash h;
                h(dbgBuf2.data() + 4, dbgBuf2.size() - 4 - 4);
                printf("LfoTrack(v31) computed-hash=0x%08x\n", h.result()); fflush(stdout);
            }
        }
        // Read with current reader (supports v32) using the full dynamic buffer
        const auto &outBuf = memoryWriter.buffer();
        MemoryReader memoryReader(outBuf.data(), outBuf.size());
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Version32);

        LfoTrack dst;
        dst.clear();
        dst.read(reader);

        // nudge should remain default (clear() default)
        expectEqual(dst.nudge(), int(0));
        expectEqual(int(dst.waveform()), int(wf));
        expectEqual(int(dst.mode()), int(md));
        expectEqual(dst.hi(), int(hi));
        expectEqual(dst.low(), int(low));
        expectEqual(dst.speed(), int(speed));
        expectEqual(dst.freeFreqCentihz(), int(freefreq));
        expectEqual(dst.pulseWidth(), int(pulseWidth));
        expectTrue(reader.checkHash());
    }
}
