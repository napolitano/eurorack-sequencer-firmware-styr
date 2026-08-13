/**
 * @file CurveTrack.h
 * @brief Declares the CurveTrack component used by the sequencer model.
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
#include "CurveSequence.h"
#include "Serialize.h"
#include "Routing.h"

/**
 * @brief Stores and manipulates curve track model data.
 */
class CurveTrack {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef std::array<CurveSequence, CONFIG_PATTERN_COUNT + CONFIG_SNAPSHOT_COUNT> CurveSequenceArray;

    // FillMode

    /**
     * @brief Enumerates the supported fill mode values.
     */
    enum class FillMode : uint8_t {
        None, ///< Represents no selection or an invalid value.
        Variation, ///< Selects the variation fill mode.
        NextPattern, ///< Selects the next pattern fill mode.
        Invert, ///< Selects the invert fill mode.
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
        case FillMode::Variation:   return TXT_MODEL_VARIATION;
        case FillMode::NextPattern: return TXT_MODEL_NEXT_PATTERN;
        case FillMode::Invert:      return TXT_MODEL_INVERT;
        case FillMode::Last:        break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported mute mode values.
     */
    enum class MuteMode : uint8_t {
        LastValue, ///< Selects the last value mute mode.
        Zero, ///< Selects the zero mute mode.
        Min, ///< Selects the min mute mode.
        Max, ///< Selects the max mute mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for mute mode.
     *
     * @param[in] muteMode Mute policy used when evaluating track output.
     *
     * @return Pointer to the mute mode name; `nullptr` when no value is available.
     */
    static const char *muteModeName(MuteMode muteMode) {
        switch (muteMode) {
        case MuteMode::LastValue:   return TXT_MODEL_LAST_VALUE;
        case MuteMode::Zero:        return TXT_MODEL_ZERO_VOLT;
        case MuteMode::Min:         return TXT_MODEL_MINIMUM;
        case MuteMode::Max:         return TXT_MODEL_MAXIMUM;
        case MuteMode::Last:        break;
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

    // muteMode

    /**
     * @brief Returns the mute mode.
     *
     * @return Configured mute mode.
     */
    MuteMode muteMode() const { return _muteMode; }
    /**
     * @brief Sets the mute mode.
     *
     * @param[in] muteMode Mute policy used when evaluating track output.
     */
    void setMuteMode(MuteMode muteMode) {
        _muteMode = ModelUtils::clampedEnum(muteMode);
    }

    /**
     * @brief Adjusts the mute mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMuteMode(int value, bool shift) {
        setMuteMode(ModelUtils::adjustedEnum(muteMode(), value));
    }

    /**
     * @brief Formats the mute mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMuteMode(StringBuilder &str) const {
        str(muteModeName(muteMode()));
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

    // offset

    /**
     * @brief Returns the offset.
     *
     * @return Configured curve-track offset.
     */
    int offset() const { return _offset.get(isRouted(Routing::Target::Offset)); }
    /**
     * @brief Offsets volts.
     *
     * @return Configured curve-track offset in volts.
     */
    float offsetVolts() const { return offset() * 0.01f; }
    /**
     * @brief Sets the offset.
     *
     * @param[in] offset Offset applied in the domain defined by the operation.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setOffset(int offset, bool routed = false) {
        _offset.set(clamp(offset, -500, 500), routed);
    }

    /**
     * @brief Adjusts the offset from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editOffset(int value, bool shift) {
        if (!isRouted(Routing::Target::Offset)) {
            setOffset(offset() + value * (shift ? 100 : 1));
        }
    }

    /**
     * @brief Formats the offset into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printOffset(StringBuilder &str) const {
        printRouted(str, Routing::Target::Offset);
        str(TXT_MODEL_OFFSET_VOLTAGE, offsetVolts());
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

    // shapeProbabilityBias

    /**
     * @brief Returns the shape probability bias.
     *
     * @return Configured bias applied to curve-shape probability.
     */
    int shapeProbabilityBias() const { return _shapeProbabilityBias.get(isRouted(Routing::Target::ShapeProbabilityBias)); }
    /**
     * @brief Sets the shape probability bias.
     *
     * @param[in] shapeProbabilityBias Shape probability bias consumed by `setShapeProbabilityBias()`.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setShapeProbabilityBias(int shapeProbabilityBias, bool routed = false) {
        _shapeProbabilityBias.set(clamp(shapeProbabilityBias, -8, 8), routed);
    }

    /**
     * @brief Adjusts the shape probability bias from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editShapeProbabilityBias(int value, bool shift) {
        if (!isRouted(Routing::Target::ShapeProbabilityBias)) {
            setShapeProbabilityBias(shapeProbabilityBias() + value);
        }
    }

    /**
     * @brief Formats the shape probability bias into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printShapeProbabilityBias(StringBuilder &str) const {
        printRouted(str, Routing::Target::ShapeProbabilityBias);
        str(TXT_MODEL_SHAPE_PROBABILITY_BIAS, shapeProbabilityBias() * 12.5f);
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
        _gateProbabilityBias.set(clamp(gateProbabilityBias, -CurveSequence::GateProbability::Range, CurveSequence::GateProbability::Range), routed);
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

    // sequences

    /**
     * @brief Returns the sequences.
     *
     * @return Reference to the sequences.
     */
    const CurveSequenceArray &sequences() const { return _sequences; }
          /**
           * @brief Returns the sequences.
           *
           * @return Reference to the sequences.
           */
          CurveSequenceArray &sequences()       { return _sequences; }

    /**
     * @brief Returns sequence.
     *
     * @param[in] index Zero-based sequence index.
     *
     * @return Reference to the sequence.
     */
    const CurveSequence &sequence(int index) const { return _sequences[index]; }
          /**
           * @brief Returns sequence.
           *
           * @param[in] index Zero-based sequence index.
           *
           * @return Reference to the sequence.
           */
          CurveSequence &sequence(int index)       { return _sequences[index]; }

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
     * @brief Constructs a CurveTrack instance.
     */
    CurveTrack() { clear(); }

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
    MuteMode _muteMode; ///< Active mute mode controlling the behavior of `CurveTrack`.
    Routable<uint8_t> _slideTime; ///< Base and routed slide time setting, resolved through the routing system when queried.
    /**
     * @brief Offset applied to  in this type's timing/coordinate domain.
     */
    Routable<int16_t> _offset; ///< Offset applied to  in this type's timing/coordinate domain.
    Routable<int8_t> _rotate; ///< Base and routed rotate setting, resolved through the routing system when queried.
    Routable<int8_t> _shapeProbabilityBias; ///< Base and routed shape probability bias setting, resolved through the routing system when queried.
    Routable<int8_t> _gateProbabilityBias; ///< Base and routed gate probability bias setting, resolved through the routing system when queried.

    CurveSequenceArray _sequences; ///< Owned fixed-size collection of sequences.

    friend class Track;
};
