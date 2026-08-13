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

/**
 * @brief Stores and manipulates lfo track model data.
 */
class LfoTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported waveform values.
     */
    enum class Waveform : uint8_t {
        Sine, ///< Selects the sine waveform.
        Triangle, ///< Selects the triangle waveform.
        RampUp, ///< Selects the ramp up waveform.
        RampDown, ///< Selects the ramp down waveform.
        Square, ///< Selects the square waveform.
        RandomHard, ///< Selects the random hard waveform.
        RandomSoft, ///< Selects the random soft waveform.
        Noise, ///< Selects the noise waveform.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for waveform.
     *
     * @param[in] waveform LFO waveform selected for evaluation or configuration.
     *
     * @return Pointer to the waveform name; `nullptr` when no value is available.
     */
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

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Sync,   ///< Restarts/advances the LFO in sequencer-step synchronization.
        Free,   ///< Runs the LFO from its local free-running phase accumulator.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the mode name; `nullptr` when no value is available.
     */
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
    /**
     * @brief Returns the waveform.
     *
     * @return Configured LFO waveform.
     */
    Waveform waveform() const { return _waveform; }
    /**
     * @brief Sets the waveform.
     *
     * @param[in] waveform LFO waveform selected for evaluation or configuration.
     */
    void setWaveform(Waveform waveform) { _waveform = waveform; }
    /**
     * @brief Adjusts the waveform from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editWaveform(int value, bool shift) { setWaveform(ModelUtils::adjustedEnum(waveform(), value)); }
    /**
     * @brief Formats the waveform into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printWaveform(StringBuilder &str) const { str(waveformName(waveform())); }

    // hi (upper amplitude)
    /**
     * @brief Returns the hi.
     *
     * @return Configured upper LFO output bound.
     */
    int hi() const { return _hi.get(isRouted(Routing::Target::LfoHi)); }
    // hi is stored in centivolts. Allow -500..500 (i.e. -5.00V..+5.00V)
    /**
     * @brief Sets the hi.
     *
     * @param[in] hi Hi consumed by `setHi()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setHi(int hi, bool routed = false) { _hi.set(clamp(hi, -500, 500), routed); }
    // edit in steps of 0.01V (10 mV) per encoder tick; shift = 10x
    // internal unit = 0.01V (1 centivolt) so 0.01V == 1 unit
    /**
     * @brief Adjusts the hi from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editHi(int value, bool shift) { if (!isRouted(Routing::Target::LfoHi)) setHi(hi() + value * (shift ? 10 : 1)); }
    /**
     * @brief Formats the hi into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printHi(StringBuilder &str) const { printRouted(str, Routing::Target::LfoHi); str("Hi: %.2fV", hi() / 100.f); }

    // low (lower amplitude)
    /**
     * @brief Returns the low.
     *
     * @return Configured lower output/value bound.
     */
    int low() const { return _low.get(isRouted(Routing::Target::LfoLow)); }
    // low is stored in centivolts. Allow -500..500 (i.e. -5.00V..+5.00V)
    /**
     * @brief Sets the low.
     *
     * @param[in] low Low consumed by `setLow()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setLow(int low, bool routed = false) { _low.set(clamp(low, -500, 500), routed); }
    // edit in steps of 0.01V (10 mV) per encoder tick; shift = 10x
    // internal unit = 0.01V (1 centivolt) so 0.01V == 1 unit
    /**
     * @brief Adjusts the low from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editLow(int value, bool shift) { if (!isRouted(Routing::Target::LfoLow)) setLow(low() + value * (shift ? 10 : 1)); }
    /**
     * @brief Formats the low into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printLow(StringBuilder &str) const { printRouted(str, Routing::Target::LfoLow); str("Low: %.2fV", low() / 100.f); }

    // mode (Sync/Free)
    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const { return _mode; }
    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode) { _mode = mode; }
    /**
     * @brief Adjusts the mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMode(int value, bool shift) { setMode(ModelUtils::adjustedEnum(mode(), value)); }
    /**
     * @brief Formats the mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMode(StringBuilder &str) const { str(modeName(mode())); }

    // speed (step length or frequency)
    /**
     * @brief Returns the speed.
     *
     * @return Configured LFO speed.
     */
    int speed() const { return _speed.get(isRouted(Routing::Target::LfoSpeed)); }
    // In Sync mode the maximum number of steps is 64, otherwise allow up to 128
    /**
     * @brief Sets the speed.
     *
     * @param[in] speed Speed consumed by `setSpeed()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setSpeed(int speed, bool routed = false) {
        int maxSpeed = (_mode == Mode::Sync) ? 64 : 128;
        _speed.set(clamp(speed, 1, maxSpeed), routed);
    }
    /**
     * @brief Adjusts the speed from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
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
    /**
     * @brief Formats the speed into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printSpeed(StringBuilder &str) const { printRouted(str, Routing::Target::LfoSpeed); if (mode() == Mode::Free) str("%.2fHz", freeFreqCentihz() / 100.f); else str("Speed: %d", speed()); }

    // Free-mode frequency in centi-Hz (0.01 Hz steps). Range 0..2000 (0..20.00 Hz)
    /**
     * @brief Returns the free freq centihz.
     *
     * @return Free freq centihz, in hertz.
     */
    int freeFreqCentihz() const { return _freeFreq.get(isRouted(Routing::Target::LfoSpeed)); }
    /**
     * @brief Sets the free freq centihz.
     *
     * @param[in] centiHz Centi hz, in hertz.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setFreeFreqCentihz(int centiHz, bool routed = false) { _freeFreq.set(clamp(centiHz, 0, 2000), routed); }

    // clip
    /**
     * @brief Clips the supplied geometry to the active bounds.
     *
     * @return `true` if clip; otherwise `false`.
     */
    bool clip() const { return _clip; }
    /**
     * @brief Sets the clip.
     *
     * @param[in] clip `true` to enable/set clip; `false` to disable/clear it.
     */
    void setClip(bool clip) { _clip = clip; }
    /**
     * @brief Adjusts the clip from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClip(int value, bool shift) { setClip(value > 0); }
    /**
     * @brief Formats the clip into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClip(StringBuilder &str) const { ModelUtils::printYesNo(str, clip()); }

    // pulse width (for Square)
    /**
     * @brief Returns the pulse width.
     *
     * @return Configured LFO pulse width.
     */
    int pulseWidth() const { return _pulseWidth.get(isRouted(Routing::Target::LfoPulseWidth)); }
    // allow 0..100% for pulse width; 0% means always low, 100% always high
    /**
     * @brief Sets the pulse width.
     *
     * @param[in] pulseWidth New pulse width in percent to store or apply; values are clamped to `0`..`100`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setPulseWidth(int pulseWidth, bool routed = false) { _pulseWidth.set(clamp(pulseWidth, 0, 100), routed); }
    /**
     * @brief Adjusts the pulse width from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editPulseWidth(int value, bool shift) { if (!isRouted(Routing::Target::LfoPulseWidth)) setPulseWidth(pulseWidth() + value * (shift ? 10 : 1)); }
    /**
     * @brief Formats the pulse width into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printPulseWidth(StringBuilder &str) const { printRouted(str, Routing::Target::LfoPulseWidth); str("PW: %d", pulseWidth()); }

    // nudge (circular step offset for Sync-mode waveforms)
    /**
     * @brief Returns the nudge.
     *
     * @return Configured LFO phase nudge.
     */
    int nudge() const { return _nudge.get(false); }
    /**
     * @brief Sets the nudge.
     *
     * @param[in] nudge Nudge consumed by `setNudge()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setNudge(int nudge, bool routed = false) { _nudge.set(nudge, routed); }
    /**
     * @brief Adjusts the nudge from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
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
    /**
     * @brief Reports whether routed.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if routed; otherwise `false`.
     */
    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    /**
     * @brief Formats the routed into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] target Target object/value affected by the operation.
     */
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    /**
     * @brief Writes routed.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] intValue Integer representation of the value.
     * @param[in] floatValue Floating-point representation of the value.
     */
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------
    /**
     * @brief Constructs a LfoTrack instance.
     */
    LfoTrack() { clear(); }
    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader);

private:
    /**
     * @brief Sets the track index.
     *
     * @param[in] trackIndex Zero-based track index.
     */
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }
    /**
     * @brief Zero-based track index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _trackIndex = -1; ///< Zero-based track index; a negative/sentinel value represents no selection where applicable.
    Waveform _waveform; ///< Selected waveform that determines how `LfoTrack` interprets or renders its data.
    Mode _mode; ///< Active mode controlling the behavior of `LfoTrack`.
    /**
     * @brief Whether clip is true in the current state.
     */
    bool _clip; ///< True when the LFO output is clipped to its configured low/high range.
    Routable<int16_t> _hi; ///< Base and routed hi setting, resolved through the routing system when queried.
    Routable<int16_t> _low; ///< Base and routed low setting, resolved through the routing system when queried.
    Routable<int16_t> _speed; ///< Base and routed speed setting, resolved through the routing system when queried.
    Routable<int8_t> _pulseWidth; ///< Base and routed pulse width setting, resolved through the routing system when queried.
    // free-mode frequency in centihertz (0.01 Hz)
    Routable<int16_t> _freeFreq; ///< Base and routed free freq setting, resolved through the routing system when queried.
    Routable<int16_t> _nudge; ///< Base and routed nudge setting, resolved through the routing system when queried.
    friend class Track;
};
