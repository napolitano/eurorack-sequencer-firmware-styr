/**
 * @file LfoTrack.cpp
 * @brief Implements the LfoTrack component used by the sequencer model.
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
#include "LfoTrack.h"
#include "ModelUtils.h"
#include "ProjectVersion.h"

void LfoTrack::clear() {
    _waveform = Waveform::Sine;
    _mode = Mode::Sync;
    _clip = false;
    _hi.set(500, false);
    _low.set(-500, false);
    setSpeed(16, false);
    // initialize free-mode frequency from default speed mapping (~2.48Hz)
    int defaultCentiHz = int(((16 - 1) * (20.0f / 127.0f)) * 100.0f + 0.5f);
    _freeFreq.set(defaultCentiHz, false);
    _pulseWidth.set(50, false);
}

void LfoTrack::writeRouted(Routing::Target target, int intValue, float floatValue) {
    // Implement routing logic if needed
}

void LfoTrack::write(VersionedSerializedWriter &writer) const {
    writer.writeEnum(_waveform, [](Waveform w) { return static_cast<uint8_t>(w); });
    writer.writeEnum(_mode, [](Mode m) { return static_cast<uint8_t>(m); });
    writer.write(_clip);
    writer.write(_hi.base);
    writer.write(_low.base);
    writer.write(_speed.base);
    writer.write(_freeFreq.base);
    writer.write(_pulseWidth.base);
    // persist nudge for Version32 and later (always write so new files include it)
    writer.write(_nudge.base);
}

void LfoTrack::read(VersionedSerializedReader &reader) {
    reader.readEnum(_waveform, [](Waveform w) { return static_cast<uint8_t>(w); });
    reader.readEnum(_mode, [](Mode m) { return static_cast<uint8_t>(m); });
    reader.read(_clip);
    int16_t hi, low, speed, freefreq;
    int8_t pulseWidth;
    reader.read(hi);
    reader.read(low);
    reader.read(speed);
    reader.read(freefreq);
    reader.read(pulseWidth);
    _hi.set(hi, false);
    _low.set(low, false);
    // Use setSpeed so the value is clamped according to the current mode (Sync has max 64)
    setSpeed(speed, false);
    _freeFreq.set(freefreq, false);
    _pulseWidth.set(pulseWidth, false);
    // read nudge if data version contains it, otherwise keep default from clear()
    if (reader.dataVersion() >= ProjectVersion::Version32) {
        int16_t nudge;
        reader.read(nudge);
        _nudge.set(nudge, false);
    }
}
