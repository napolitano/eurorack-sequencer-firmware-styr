/**
 * @file MidiCvTrack.h
 * @brief Declares the MidiCvTrack component used by the sequencer model.
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
#pragma once

#include "Config.h"
#include "Types.h"
#include "MidiConfig.h"
#include "ModelUtils.h"
#include "Serialize.h"
#include "Arpeggiator.h"
#include "Routing.h"

#include "core/math/Math.h"

/**
 * @brief Stores and manipulates midi cv track model data.
 */
class MidiCvTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported voice signal values.
     */
    enum class VoiceSignal : uint8_t { Pitch, Velocity, Pressure };

    /**
     * @brief Enumerates the supported voice config values.
     */
    enum class VoiceConfig : uint8_t {
        Pitch, ///< Selects the pitch voice config.
        Velocity, ///< Selects the velocity voice config.
        PitchVelocity, ///< Selects the pitch velocity voice config.
        PitchVelocityPressure, ///< Selects the pitch velocity pressure voice config.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for voice config.
     *
     * @param[in] voiceConfig Voice-allocation configuration applied to the MIDI-CV track.
     *
     * @return Pointer to the voice config name; `nullptr` when no value is available.
     */
    static const char *voiceConfigName(VoiceConfig voiceConfig) {
        switch (voiceConfig) {
        case VoiceConfig::Pitch:                    return TXT_MODEL_PITCH;
        case VoiceConfig::Velocity:                 return TXT_MODEL_VELOCITY;
        case VoiceConfig::PitchVelocity:            return TXT_MODEL_PITCH_VELOCITY;
        case VoiceConfig::PitchVelocityPressure:    return TXT_MODEL_PITCH_VELOCITY_PRESSURE;
        case VoiceConfig::Last:                     break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported note priority values.
     */
    enum class NotePriority : uint8_t {
        LastNote, ///< Selects the last note note priority.
        FirstNote, ///< Selects the first note note priority.
        LowestNote, ///< Selects the lowest note note priority.
        HighestNote, ///< Selects the highest note note priority.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for note priority.
     *
     * @param[in] notePriority Priority policy used when selecting notes for available voices.
     *
     * @return Pointer to the note priority name; `nullptr` when no value is available.
     */
    static const char *notePriorityName(NotePriority notePriority) {
        switch (notePriority) {
        case NotePriority::LastNote:    return TXT_MODEL_LAST_NOTE;
        case NotePriority::FirstNote:   return TXT_MODEL_FIRST_NOTE;
        case NotePriority::LowestNote:  return TXT_MODEL_LOWEST_NOTE;
        case NotePriority::HighestNote: return TXT_MODEL_HIGHEST_NOTE;
        case NotePriority::Last:        break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // source

    /**
     * @brief Returns the source.
     *
     * @return Reference to the source.
     */
    const MidiSourceConfig &source() const { return _source; }
          /**
           * @brief Returns the source.
           *
           * @return Reference to the source.
           */
          MidiSourceConfig &source()       { return _source; }

    // voices

    /**
     * @brief Returns the voices.
     *
     * @return Configured MIDI-to-CV voice count.
     */
    int voices() const { return _voices; }
    /**
     * @brief Sets the voices.
     *
     * @param[in] voices Number of simultaneously allocated MIDI-to-CV voices.
     */
    void setVoices(int voices) {
        _voices = clamp(voices, 1, 8);
    }

    /**
     * @brief Adjusts the voices from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editVoices(int value, bool shift) {
        setVoices(voices() + value);
    }

    /**
     * @brief Formats the voices into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printVoices(StringBuilder &str) const {
        str(TXT_MODEL_PRINT_VOICES, voices());
    }

    // voiceConfig

    /**
     * @brief Returns the voice config.
     *
     * @return Configuration for the addressed MIDI-to-CV voice.
     */
    VoiceConfig voiceConfig() const { return _voiceConfig; }
    /**
     * @brief Sets the voice config.
     *
     * @param[in] voiceConfig Voice-allocation configuration applied to the MIDI-CV track.
     */
    void setVoiceConfig(VoiceConfig voiceConfig) {
        _voiceConfig = ModelUtils::clampedEnum(voiceConfig);
    }

    /**
     * @brief Adjusts the voice config from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editVoiceConfig(int value, bool shift) {
        setVoiceConfig(ModelUtils::adjustedEnum(voiceConfig(), value));
    }

    /**
     * @brief Formats the voice config into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printVoiceConfig(StringBuilder &str) const {
        str(voiceConfigName(_voiceConfig));
    }

    // notePriority

    /**
     * @brief Returns the note priority.
     *
     * @return Configured note-allocation priority.
     */
    NotePriority notePriority() const { return _notePriority; }
    /**
     * @brief Sets the note priority.
     *
     * @param[in] notePriority Priority policy used when selecting notes for available voices.
     */
    void setNotePriority(NotePriority notePriority) {
        _notePriority = notePriority;
    }

    /**
     * @brief Adjusts the note priority from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editNotePriority(int value, bool shift) {
        setNotePriority(ModelUtils::adjustedEnum(notePriority(), value));
    }

    /**
     * @brief Formats the note priority into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printNotePriority(StringBuilder &str) const {
        str(notePriorityName(notePriority()));
    }

    // lowNote

    /**
     * @brief Returns the low note.
     *
     * @return Lowest accepted MIDI note.
     */
    int lowNote() const { return _lowNote; }
    /**
     * @brief Sets the low note.
     *
     * @param[in] lowNote New lowest accepted MIDI note number to store or apply.
     */
    void setLowNote(int lowNote) {
        _lowNote = clamp(lowNote, 0, highNote());
    }

    /**
     * @brief Adjusts the low note from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editLowNote(int value, bool shift) {
        setLowNote(lowNote() + value * (shift ? 12 : 1));
    }

    /**
     * @brief Formats the low note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printLowNote(StringBuilder &str) const {
        Types::printMidiNote(str, lowNote());
    }

    // highNote

    /**
     * @brief Returns the high note.
     *
     * @return Highest accepted MIDI note.
     */
    int highNote() const { return _highNote; }
    /**
     * @brief Sets the high note.
     *
     * @param[in] highNote New highest accepted MIDI note number to store or apply.
     */
    void setHighNote(int highNote) {
        _highNote = clamp(highNote, lowNote(), 127);
    }

    /**
     * @brief Adjusts the high note from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editHighNote(int value, bool shift) {
        setHighNote(highNote() + value * (shift ? 12 : 1));
    }

    /**
     * @brief Formats the high note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printHighNote(StringBuilder &str) const {
        Types::printMidiNote(str, highNote());
    }

    // pitchBendRange

    /**
     * @brief Returns the pitch bend range.
     *
     * @return Configured pitch-bend range.
     */
    int pitchBendRange() const { return _pitchBendRange; }
    /**
     * @brief Sets the pitch bend range.
     *
     * @param[in] pitchBendRange New pitch-bend range in semitones to store or apply.
     */
    void setPitchBendRange(int pitchBendRange) {
        _pitchBendRange = clamp(pitchBendRange, 0, 48);
    }

    /**
     * @brief Adjusts the pitch bend range from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editPitchBendRange(int value, bool shift) {
        setPitchBendRange(pitchBendRange() + value);
    }

    /**
     * @brief Formats the pitch bend range into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printPitchBendRange(StringBuilder &str) const {
        if (_pitchBendRange == 0) {
            str(TXT_MODEL_PRINT_OFF);
        } else {
            str(TXT_MODEL_PRINT_SEMITONES, _pitchBendRange);
        }
    }

    // modulationRange

    /**
     * @brief Returns the modulation range.
     *
     * @return Configured modulation-output range.
     */
    Types::VoltageRange modulationRange() const { return _modulationRange; }
    /**
     * @brief Sets the modulation range.
     *
     * @param[in] modulationRange Modulation range consumed by `setModulationRange()`.
     */
    void setModulationRange(Types::VoltageRange modulationRange) {
        _modulationRange = ModelUtils::clampedEnum(modulationRange);
    }

    /**
     * @brief Adjusts the modulation range from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editModulationRange(int value, bool shift) {
        setModulationRange(ModelUtils::adjustedEnum(modulationRange(), value));
    }

    /**
     * @brief Formats the modulation range into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printModulationRange(StringBuilder &str) const {
        str(Types::voltageRangeName(modulationRange()));
    }

    // retrigger

    /**
     * @brief Returns the retrigger.
     *
     * @return `true` if retrigger; otherwise `false`.
     */
    bool retrigger() const { return _retrigger; }
    /**
     * @brief Sets the retrigger.
     *
     * @param[in] retrigger `true` to enable/set retrigger; `false` to disable/clear it.
     */
    void setRetrigger(bool retrigger) {
        _retrigger = retrigger;
    }

    /**
     * @brief Adjusts the retrigger from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRetrigger(int value, bool shift) {
        setRetrigger(value > 0);
    }

    /**
     * @brief Formats the retrigger into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRetrigger(StringBuilder &str) const {
        ModelUtils::printYesNo(str, retrigger());
    }

    // slideTime

    /**
     * @brief Returns the slide time.
     *
     * @return Configured slide/glide time.
     */
    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    /**
     * @brief Sets the slide time.
     *
     * @param[in] slideTime Slide time consumed by `setSlideTime()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    /**
     * @brief Adjusts the slide time from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editSlideTime(int value, bool shift) {
        if (!isRouted(Routing::Target::SlideTime)) {
            setSlideTime(ModelUtils::adjustedByStep(slideTime(), value, 5, !shift));
        }
    }

    /**
     * @brief Formats the slide time into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printSlideTime(StringBuilder &str) const {
        printRouted(str, Routing::Target::SlideTime);
        str(TXT_MODEL_SLIDE_TIME, slideTime());
    }

    // transpose

    /**
     * @brief Returns the transpose.
     *
     * @return Configured transposition amount.
     */
    int transpose() const { return _transpose.get(isRouted(Routing::Target::Transpose)); }
    /**
     * @brief Sets the transpose.
     *
     * @param[in] transpose New transposition amount in semitones to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setTranspose(int transpose, bool routed = false) {
        _transpose.set(clamp(transpose, -100, 100), routed);
    }

    /**
     * @brief Adjusts the transpose from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editTranspose(int value, bool shift) {
        if (!isRouted(Routing::Target::Transpose)) {
            setTranspose(transpose() + value);
        }
    }

    /**
     * @brief Formats the transpose into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printTranspose(StringBuilder &str) const {
        printRouted(str, Routing::Target::Transpose);
        str(TXT_MODEL_TRANSPOSE, transpose());
    }

    // arpeggiator

    /**
     * @brief Returns the arpeggiator.
     *
     * @return Reference to the arpeggiator.
     */
    const Arpeggiator &arpeggiator() const { return _arpeggiator; }
          /**
           * @brief Returns the arpeggiator.
           *
           * @return Reference to the arpeggiator.
           */
          Arpeggiator &arpeggiator()       { return _arpeggiator; }

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
     * @brief Constructs a MidiCvTrack instance.
     */
    MidiCvTrack() { clear(); }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Returns the voice signal count.
     *
     * @return Number of voice signal entries represented by the object.
     */
    int voiceSignalCount() const;
    /**
     * @brief Returns voice signal by index.
     *
     * @param[in] index Zero-based voice signal by index index.
     *
     * @return Result of voiceSignalByIndex().
     */
    VoiceSignal voiceSignalByIndex(int index) const;

    /**
     * @brief Returns the display name of a gate output.
     *
     * @param[in] index Zero-based gate name index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void gateOutputName(int index, StringBuilder &str) const;
    /**
     * @brief Returns the display name of a CV output.
     *
     * @param[in] index Zero-based cv name index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void cvOutputName(int index, StringBuilder &str) const;

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
    void setTrackIndex(int trackIndex) {
        _trackIndex = trackIndex;
    }

    /**
     * @brief Zero-based track index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _trackIndex = -1; ///< Zero-based track index; a negative/sentinel value represents no selection where applicable.
    MidiSourceConfig _source; ///< MIDI routing/source configuration.
    uint8_t _voices; ///< Number of simultaneously allocated MIDI-to-CV voices, constrained to 1..8.
    VoiceConfig _voiceConfig; ///< Voice-to-output allocation scheme used by the MIDI-to-CV track.
    NotePriority _notePriority; ///< Note-stealing/selection priority used when more notes are held than voices are available.
    uint8_t _lowNote; ///< Lowest accepted MIDI note number; constrained to 0..`highNote()`.
    uint8_t _highNote; ///< Highest accepted MIDI note number; constrained to `lowNote()`..127.
    uint8_t _pitchBendRange; ///< Pitch-bend span in semitones, constrained to 0..48.
    Types::VoltageRange _modulationRange; ///< Configured CV voltage range.
    /**
     * @brief Whether retrigger is true in the current state.
     */
    bool _retrigger; ///< True when repeated notes retrigger the generated gate instead of remaining legato.
    Routable<uint8_t> _slideTime; ///< Base and routed slide time setting, resolved through the routing system when queried.
    Routable<int8_t> _transpose; ///< Base and routed transpose setting, resolved through the routing system when queried.
    Arpeggiator _arpeggiator; ///< Persistent arpeggiator configuration embedded in this MIDI-CV track.

    friend class Track;
};
