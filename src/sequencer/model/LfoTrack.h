/**
 * @file LfoTrack.h
 * @brief Declares the LfoTrack component used by the sequencer model.
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
#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "Routing.h"
#include <algorithm>

class LfoTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class Waveform : uint8_t {
        Sine,
        Triangle,
        RampUp,
        RampDown,
        Square,
        RandomHard,
        RandomSoft,
        Noise,
        Last
    };

    static const char *waveformName(Waveform waveform) {
        switch (waveform) {
        case Waveform::Sine:      return "Sine";
        case Waveform::Triangle:  return "Triangle";
        case Waveform::RampUp:    return "Ramp Up";
        case Waveform::RampDown:  return "Ramp Down";
        case Waveform::Square:    return "Square";
        case Waveform::RandomHard: return "Random";
        case Waveform::RandomSoft: return "Smoothed Random";
        case Waveform::Noise:     return "Noise";
        case Waveform::Last:      break;
        }
        return nullptr;
    }

    enum class Mode : uint8_t {
        Sync,   // Step-synced
        Free,   // Free-running
        Last
    };

    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::Sync: return "Sync";
        case Mode::Free: return "Free";
        case Mode::Last: break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // waveform
    Waveform waveform() const { return _waveform; }
    void setWaveform(Waveform waveform) { _waveform = waveform; }
    void editWaveform(int value, bool shift) { setWaveform(ModelUtils::adjustedEnum(waveform(), value)); }
    void printWaveform(StringBuilder &str) const { str(waveformName(waveform())); }

    // hi (upper amplitude)
    int hi() const { return _hi.get(isRouted(Routing::Target::LfoHi)); }
    // hi is stored in centivolts. Allow -500..500 (i.e. -5.00V..+5.00V)
    void setHi(int hi, bool routed = false) { _hi.set(clamp(hi, -500, 500), routed); }
    // edit in steps of 0.01V (10 mV) per encoder tick; shift = 10x
    // internal unit = 0.01V (1 centivolt) so 0.01V == 1 unit
    void editHi(int value, bool shift) { if (!isRouted(Routing::Target::LfoHi)) setHi(hi() + value * (shift ? 10 : 1)); }
    void printHi(StringBuilder &str) const { printRouted(str, Routing::Target::LfoHi); str("Hi: %.2fV", hi() / 100.f); }

    // low (lower amplitude)
    int low() const { return _low.get(isRouted(Routing::Target::LfoLow)); }
    // low is stored in centivolts. Allow -500..500 (i.e. -5.00V..+5.00V)
    void setLow(int low, bool routed = false) { _low.set(clamp(low, -500, 500), routed); }
    // edit in steps of 0.01V (10 mV) per encoder tick; shift = 10x
    // internal unit = 0.01V (1 centivolt) so 0.01V == 1 unit
    void editLow(int value, bool shift) { if (!isRouted(Routing::Target::LfoLow)) setLow(low() + value * (shift ? 10 : 1)); }
    void printLow(StringBuilder &str) const { printRouted(str, Routing::Target::LfoLow); str("Low: %.2fV", low() / 100.f); }

    // mode (Sync/Free)
    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }
    void editMode(int value, bool shift) { setMode(ModelUtils::adjustedEnum(mode(), value)); }
    void printMode(StringBuilder &str) const { str(modeName(mode())); }

    // speed (step length or frequency)
    int speed() const { return _speed.get(isRouted(Routing::Target::LfoSpeed)); }
    // In Sync mode the maximum number of steps is 64, otherwise allow up to 128
    void setSpeed(int speed, bool routed = false) {
        int maxSpeed = (_mode == Mode::Sync) ? 64 : 128;
        _speed.set(clamp(speed, 1, maxSpeed), routed);
    }
    void editSpeed(int value, bool shift) {
        if (!isRouted(Routing::Target::LfoSpeed)) {
            if (mode() == Mode::Free) {
                // In Free mode: adjust frequency in centi-Hz (0.01 Hz)
                int delta = value * (shift ? 10 : 1); // encoder tick -> centiHz steps
                int newCenti = freeFreqCentihz() + delta;
                setFreeFreqCentihz(newCenti);
            } else {
                int delta = value * (shift ? 10 : 1);
                int newSpeed = speed() + delta;
                int maxSpeed = (mode() == Mode::Sync) ? 64 : 128;
                newSpeed = clamp(newSpeed, 1, maxSpeed);
                setSpeed(newSpeed);
            }
        }
    }
    void printSpeed(StringBuilder &str) const { printRouted(str, Routing::Target::LfoSpeed); if (mode() == Mode::Free) str("%.2fHz", freeFreqCentihz() / 100.f); else str("Speed: %d", speed()); }

    // Free-mode frequency in centi-Hz (0.01 Hz steps). Range 0..2000 (0..20.00 Hz)
    int freeFreqCentihz() const { return _freeFreq.get(isRouted(Routing::Target::LfoSpeed)); }
    void setFreeFreqCentihz(int centiHz, bool routed = false) { _freeFreq.set(clamp(centiHz, 0, 2000), routed); }

    // clip
    bool clip() const { return _clip; }
    void setClip(bool clip) { _clip = clip; }
    void editClip(int value, bool shift) { setClip(value > 0); }
    void printClip(StringBuilder &str) const { ModelUtils::printYesNo(str, clip()); }

    // pulse width (for Square)
    int pulseWidth() const { return _pulseWidth.get(isRouted(Routing::Target::LfoPulseWidth)); }
    // allow 0..100% for pulse width; 0% means always low, 100% always high
    void setPulseWidth(int pulseWidth, bool routed = false) { _pulseWidth.set(clamp(pulseWidth, 0, 100), routed); }
    void editPulseWidth(int value, bool shift) { if (!isRouted(Routing::Target::LfoPulseWidth)) setPulseWidth(pulseWidth() + value * (shift ? 10 : 1)); }
    void printPulseWidth(StringBuilder &str) const { printRouted(str, Routing::Target::LfoPulseWidth); str("PW: %d", pulseWidth()); }

    // nudge (circular step offset for Sync-mode waveforms)
    int nudge() const { return _nudge.get(false); }
    void setNudge(int nudge, bool routed = false) { _nudge.set(nudge, routed); }
    void editNudge(int value, bool shift) {
        // only active in Sync mode and for certain waveforms
        if (mode() != Mode::Sync) return;
        switch (waveform()) {
            case Waveform::Sine:
            case Waveform::Triangle:
            case Waveform::RampUp:
            case Waveform::RampDown:
            case Waveform::Square:
                break;
            default:
                return;
        }
        int delta = value * (shift ? 10 : 1);
        int maxSteps = std::max(1, speed());
        int newNudge = nudge() + delta;
        newNudge = clamp(newNudge, -maxSteps, maxSteps);
        _nudge.set(newNudge, false);
    }

    //----------------------------------------
    // Routing
    //----------------------------------------
    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------
    LfoTrack() { clear(); }
    void clear();
    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }
    int8_t _trackIndex = -1;
    Waveform _waveform;
    Mode _mode;
    bool _clip;
    Routable<int16_t> _hi;
    Routable<int16_t> _low;
    Routable<int16_t> _speed;
    Routable<int8_t> _pulseWidth;
    // free-mode frequency in centihertz (0.01 Hz)
    Routable<int16_t> _freeFreq;
    Routable<int16_t> _nudge;
    friend class Track;
};
