/**
 * @file NoteTrack.h
 * @brief Declares the NoteTrack component used by the sequencer model.
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
#include "NoteSequence.h"
#include "Serialize.h"
#include "Routing.h"

/**
 * @brief Stores and manipulates note track model data.
 */
class NoteTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<NoteSequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> NoteSequenceArray;

    // FillMode

    /**
     * @brief Enumerates the supported fill mode values.
     */
    enum class FillMode : uint8_t {
        None, ///< Represents no selection or an invalid value.
        Gates, ///< Selects the gates fill mode.
        NextPattern, ///< Selects the next pattern fill mode.
        Condition, ///< Selects the condition fill mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Fills mode name.
     *
     * @param[in] fillMode Fill behavior selected for the track/sequence.
     *
     * @return Pointer to the fill mode name; `nullptr` when no value is available.
     */
    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:        return TXT_MODEL_NONE;
        case FillMode::Gates:       return TXT_MODEL_GATE_PLURAL;
        case FillMode::NextPattern: return TXT_MODEL_NEXT_PATTERN;
        case FillMode::Condition:   return TXT_MODEL_NOTE_CONDITION;
        case FillMode::Last:        break;
        }
        return nullptr;
    }

    // CvUpdateMode

    /**
     * @brief Enumerates the supported cv update mode values.
     */
    enum class CvUpdateMode : uint8_t {
        Gate, ///< Selects the gate cv update mode.
        Always, ///< Selects the always cv update mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for cv update mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the cv update mode name; `nullptr` when no value is available.
     */
    static const char *cvUpdateModeName(CvUpdateMode mode) {
        switch (mode) {
        case CvUpdateMode::Gate:    return TXT_MODEL_GATE;
        case CvUpdateMode::Always:  return TXT_MODEL_ALWAYS;
        case CvUpdateMode::Last:    break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // playMode

    /**
     * @brief Returns the play mode.
     *
     * @return Configured play mode.
     */
    Types::PlayMode playMode() const { return _playMode; }
    /**
     * @brief Sets the play mode.
     *
     * @param[in] playMode Playback mode that determines transport behavior.
     */
    void setPlayMode(Types::PlayMode playMode) {
        _playMode = ModelUtils::clampedEnum(playMode);
    }

    /**
     * @brief Adjusts the play mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editPlayMode(int value, bool shift) {
        setPlayMode(ModelUtils::adjustedEnum(playMode(), value));
    }

    /**
     * @brief Formats the play mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printPlayMode(StringBuilder &str) const {
        str(Types::playModeName(playMode()));
    }

    // fillMode

    /**
     * @brief Fills mode.
     *
     * @return Configured fill mode.
     */
    FillMode fillMode() const { return _fillMode; }
    /**
     * @brief Sets the fill mode.
     *
     * @param[in] fillMode Fill behavior selected for the track/sequence.
     */
    void setFillMode(FillMode fillMode) {
        _fillMode = ModelUtils::clampedEnum(fillMode);
    }

    /**
     * @brief Adjusts the fill mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editFillMode(int value, bool shift) {
        setFillMode(ModelUtils::adjustedEnum(fillMode(), value));
    }

    /**
     * @brief Formats the fill mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printFillMode(StringBuilder &str) const {
        str(fillModeName(fillMode()));
    }

    // fillMuted

    /**
     * @brief Fills muted.
     *
     * @return `true` if fill muted; otherwise `false`.
     */
    bool fillMuted() const { return _fillMuted; }
    /**
     * @brief Sets the fill muted.
     *
     * @param[in] fillMuted `true` to enable/set fill muted; `false` to disable/clear it.
     */
    void setFillMuted(bool fillMuted) {
        _fillMuted = fillMuted;
    }

    /**
     * @brief Adjusts the fill muted from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editFillMuted(int value, bool shift) {
        setFillMuted(value > 0);
    }

    /**
     * @brief Formats the fill muted into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printFillMuted(StringBuilder &str) const {
        ModelUtils::printYesNo(str, fillMuted());
    }

    // cvUpdateMode

    /**
     * @brief Returns the cv update mode.
     *
     * @return Configured cv update mode.
     */
    CvUpdateMode cvUpdateMode() const { return _cvUpdateMode; }
    /**
     * @brief Sets the cv update mode.
     *
     * @param[in] cvUpdateMode Cv update mode consumed by `setCvUpdateMode()`.
     */
    void setCvUpdateMode(CvUpdateMode cvUpdateMode) {
        _cvUpdateMode = ModelUtils::clampedEnum(cvUpdateMode);
    }

    /**
     * @brief Adjusts the cv update mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editCvUpdateMode(int value, bool shift) {
        setCvUpdateMode(ModelUtils::adjustedEnum(cvUpdateMode(), value));
    }

    /**
     * @brief Formats the cv update mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printCvUpdateMode(StringBuilder &str) const {
        str(cvUpdateModeName(cvUpdateMode()));
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

    // octave

    /**
     * @brief Returns the octave.
     *
     * @return Configured track octave offset.
     */
    int octave() const { return _octave.get(isRouted(Routing::Target::Octave)); }
    /**
     * @brief Sets the octave.
     *
     * @param[in] octave New octave offset to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setOctave(int octave, bool routed = false) {
        _octave.set(clamp(octave, -10, 10), routed);
    }

    /**
     * @brief Adjusts the octave from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editOctave(int value, bool shift) {
        if (!isRouted(Routing::Target::Octave)) {
            setOctave(octave() + value);
        }
    }

    /**
     * @brief Formats the octave into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printOctave(StringBuilder &str) const {
        printRouted(str, Routing::Target::Octave);
        str(TXT_MODEL_OCTAVE, octave());
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

    // rotate

    /**
     * @brief Returns the rotate.
     *
     * @return Configured curve-sequence rotation.
     */
    int rotate() const { return _rotate.get(isRouted(Routing::Target::Rotate)); }
    /**
     * @brief Sets the rotate.
     *
     * @param[in] rotate Rotate consumed by `setRotate()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setRotate(int rotate, bool routed = false) {
        _rotate.set(clamp(rotate, -64, 64), routed);
    }

    /**
     * @brief Adjusts the rotate from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRotate(int value, bool shift) {
        if (!isRouted(Routing::Target::Rotate)) {
            setRotate(rotate() + value);
        }
    }

    /**
     * @brief Formats the rotate into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRotate(StringBuilder &str) const {
        printRouted(str, Routing::Target::Rotate);
        str(TXT_MODEL_ROTATION, rotate());
    }

    // gateProbabilityBias

    /**
     * @brief Returns the gate probability bias.
     *
     * @return Configured bias applied to curve-gate probability.
     */
    int gateProbabilityBias() const { return _gateProbabilityBias.get(isRouted(Routing::Target::GateProbabilityBias)); }
    /**
     * @brief Sets the gate probability bias.
     *
     * @param[in] gateProbabilityBias Gate probability bias consumed by `setGateProbabilityBias()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setGateProbabilityBias(int gateProbabilityBias, bool routed = false) {
        _gateProbabilityBias.set(clamp(gateProbabilityBias, -NoteSequence::GateProbability::Range, NoteSequence::GateProbability::Range), routed);
    }

    /**
     * @brief Adjusts the gate probability bias from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editGateProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::GateProbabilityBias)) {
            setGateProbabilityBias(gateProbabilityBias() + value);
        }
    }

    /**
     * @brief Formats the gate probability bias into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printGateProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::GateProbabilityBias);
        str(TXT_MODEL_GATE_PROBABILITY_BIAS, gateProbabilityBias() * 12.5f);
    }

    // retriggerProbabilityBias

    /**
     * @brief Returns the retrigger probability bias.
     *
     * @return Configured bias applied to note retrigger probability.
     */
    int retriggerProbabilityBias() const { return _retriggerProbabilityBias.get(isRouted(Routing::Target::RetriggerProbabilityBias)); }
    /**
     * @brief Sets the retrigger probability bias.
     *
     * @param[in] retriggerProbabilityBias Retrigger probability bias consumed by `setRetriggerProbabilityBias()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setRetriggerProbabilityBias(int retriggerProbabilityBias, bool routed = false) {
        _retriggerProbabilityBias.set(clamp(retriggerProbabilityBias, -NoteSequence::RetriggerProbability::Range, NoteSequence::RetriggerProbability::Range), routed);
    }

    /**
     * @brief Adjusts the retrigger probability bias from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRetriggerProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::RetriggerProbabilityBias)) {
            setRetriggerProbabilityBias(retriggerProbabilityBias() + value);
        }
    }

    /**
     * @brief Formats the retrigger probability bias into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRetriggerProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::RetriggerProbabilityBias);
        str(TXT_MODEL_RETRIGGER_PROBABILITY_BIAS, retriggerProbabilityBias() * 12.5f);
    }

    // lengthBias

    /**
     * @brief Returns the length bias.
     *
     * @return Configured bias applied to note gate length.
     */
    int lengthBias() const { return _lengthBias.get(isRouted(Routing::Target::LengthBias)); }
    /**
     * @brief Sets the length bias.
     *
     * @param[in] lengthBias Length bias consumed by `setLengthBias()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setLengthBias(int lengthBias, bool routed = false) {
        _lengthBias.set(clamp(lengthBias, -NoteSequence::Length::Range, NoteSequence::Length::Range), routed);
    }

    /**
     * @brief Adjusts the length bias from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editLengthBias(int value, bool shift) {
        if (!isRouted(Routing::Target::LengthBias)) {
            setLengthBias(lengthBias() + value);
        }
    }

    /**
     * @brief Formats the length bias into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printLengthBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::LengthBias);
        str(TXT_MODEL_LENGTH_BIAS, lengthBias() * 12.5f);
    }

    // noteProbabilityBias

    /**
     * @brief Returns the note probability bias.
     *
     * @return Configured bias applied to note probability.
     */
    int noteProbabilityBias() const { return _noteProbabilityBias.get(isRouted(Routing::Target::NoteProbabilityBias)); }
    /**
     * @brief Sets the note probability bias.
     *
     * @param[in] noteProbabilityBias Note probability bias consumed by `setNoteProbabilityBias()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setNoteProbabilityBias(int noteProbabilityBias, bool routed = false) {
        _noteProbabilityBias.set(clamp(noteProbabilityBias, -NoteSequence::NoteVariationProbability::Range, NoteSequence::NoteVariationProbability::Range), routed);
    }

    /**
     * @brief Adjusts the note probability bias from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editNoteProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::NoteProbabilityBias)) {
            setNoteProbabilityBias(noteProbabilityBias() + value);
        }
    }

    /**
     * @brief Formats the note probability bias into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printNoteProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::NoteProbabilityBias);
        str(TXT_MODEL_NOTE_PROBABILITY_BIAS, noteProbabilityBias() * 12.5f);
    }

    // sequences

    /**
     * @brief Returns the sequences.
     *
     * @return Reference to the sequences.
     */
    const NoteSequenceArray &sequences() const { return _sequences; }
          /**
           * @brief Returns the sequences.
           *
           * @return Reference to the sequences.
           */
          NoteSequenceArray &sequences()       { return _sequences; }

    /**
     * @brief Returns sequence.
     *
     * @param[in] index Zero-based sequence index.
     *
     * @return Reference to the sequence.
     */
    const NoteSequence &sequence(int index) const { return _sequences[index]; }
          /**
           * @brief Returns sequence.
           *
           * @param[in] index Zero-based sequence index.
           *
           * @return Reference to the sequence.
           */
          NoteSequence &sequence(int index)       { return _sequences[index]; }

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
     * @brief Constructs a NoteTrack instance.
     */
    NoteTrack() { clear(); }

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
    void setTrackIndex(int trackIndex) {
        _trackIndex = trackIndex;
        for (auto &sequence : _sequences) {
            sequence.setTrackIndex(trackIndex);
        }
    }

    /**
     * @brief Zero-based track index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _trackIndex = -1; ///< Zero-based track index; a negative/sentinel value represents no selection where applicable.
    Types::PlayMode _playMode; ///< Configured playback mode.
    FillMode _fillMode; ///< Configured fill behavior.
    /**
     * @brief Whether fill muted is true in the current state.
     */
    bool _fillMuted; ///< True when the track is muted specifically during an active fill.
    CvUpdateMode _cvUpdateMode; ///< Active cv update mode controlling the behavior of `NoteTrack`.
    Routable<uint8_t> _slideTime; ///< Base and routed slide time setting, resolved through the routing system when queried.
    Routable<int8_t> _octave; ///< Base and routed octave setting, resolved through the routing system when queried.
    Routable<int8_t> _transpose; ///< Base and routed transpose setting, resolved through the routing system when queried.
    Routable<int8_t> _rotate; ///< Base and routed rotate setting, resolved through the routing system when queried.
    Routable<int8_t> _gateProbabilityBias; ///< Base and routed gate probability bias setting, resolved through the routing system when queried.
    Routable<int8_t> _retriggerProbabilityBias; ///< Base and routed retrigger probability bias setting, resolved through the routing system when queried.
    Routable<int8_t> _lengthBias; ///< Base and routed length bias setting, resolved through the routing system when queried.
    Routable<int8_t> _noteProbabilityBias; ///< Base and routed note probability bias setting, resolved through the routing system when queried.

    NoteSequenceArray _sequences; ///< Owned fixed-size collection of sequences.

    friend class Track;
};
