/**
 * @file Track.h
 * @brief Declares the Track component used by the sequencer model.
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
#include "Serialize.h"
#include "ModelUtils.h"
#include "NoteTrack.h"
#include "CurveTrack.h"
#include "MidiCvTrack.h"
#include "LfoTrack.h"

#include "core/Debug.h"
#include "core/math/Math.h"
#include "core/utils/StringUtils.h"
#include "core/utils/Container.h"

#include <cstdint>
#include <cstring>

#if CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) ASSERT(_actual_ == _expected_, "invalid track mode");
#else // CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) {}
#endif // CONFIG_ENABLE_SANITIZE

class Project;

/**
 * @brief Stores and manipulates track model data.
 */
class Track {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported track mode values.
     */
    enum class TrackMode : uint8_t {
        Note, ///< Selects the note track mode.
        Curve, ///< Selects the curve track mode.
        MidiCv, ///< Selects the midi cv track mode.
        Lfo, ///< Selects the lfo track mode.
        Last, ///< Sentinel marking the end of the valid enumeration range.
        Default = Note ///< Selects the default track mode.
    };

    /**
     * @brief Returns the display name for track mode.
     *
     * @param[in] trackMode Track mode used to select model/engine behavior.
     *
     * @return Pointer to the track mode name; `nullptr` when no value is available.
     */
    static const char *trackModeName(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:   return TXT_LIST_LABEL_NOTE;
        case TrackMode::Curve:  return TXT_LIST_LABEL_CURVE;
        case TrackMode::MidiCv: return TXT_LIST_LABEL_MIDI_CV;
        case TrackMode::Lfo:    return "LFO"; // TODO: ggf. TXT_LIST_LABEL_LFO
        case TrackMode::Last:   break;
        }
        return nullptr;
    }

    /**
     * @brief Returns track mode serialize.
     *
     * @param[in] trackMode Track mode used to select model/engine behavior.
     *
     * @return Result of trackModeSerialize().
     */
    static uint8_t trackModeSerialize(TrackMode trackMode) {
        switch (trackMode) {
        case TrackMode::Note:   return 0;
        case TrackMode::Curve:  return 1;
        case TrackMode::MidiCv: return 2;
        case TrackMode::Lfo:    return 3;
        case TrackMode::Last:   break;
        }
        return 0;
    }


    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    /**
     * @brief Returns the track index.
     *
     * @return Zero-based track index.
     */
    int trackIndex() const { return _trackIndex; }

    // trackMode

    /**
     * @brief Returns the track mode implemented by this engine.
     *
     * @return Configured track mode.
     */
    TrackMode trackMode() const { return _trackMode; }

    /**
     * @brief Formats the track mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printTrackMode(StringBuilder &str) const {
        str(trackModeName(trackMode()));
    }

    // linkTrack

    /**
     * @brief Returns the link track.
     *
     * @return Track selected as the synchronization/link source.
     */
    int linkTrack() const { return _linkTrack; }
    /**
     * @brief Sets the link track.
     *
     * @param[in] linkTrack Link track consumed by `setLinkTrack()`.
     */
    void setLinkTrack(int linkTrack) {
        _linkTrack = clamp(linkTrack, -1, _trackIndex - 1);
    }

    /**
     * @brief Adjusts the link track from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editLinkTrack(int value, bool shift) {
        setLinkTrack(linkTrack() + value);
    }

    /**
     * @brief Formats the link track into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printLinkTrack(StringBuilder &str) const {
        if (linkTrack() == -1) {
            str(TXT_LIST_LABEL_NONE);
        } else {
            str(TXT_LIST_LABEL_TRACK, linkTrack() + 1);
        }
    }

    // noteTrack
    /**
     * @brief Returns the note track.
     *
     * @return Reference to the note track.
     */
    const NoteTrack &noteTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }
          /**
           * @brief Returns the note track.
           *
           * @return Reference to the note track.
           */
          NoteTrack &noteTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }

    // curveTrack
    /**
     * @brief Returns the curve track.
     *
     * @return Reference to the curve track.
     */
    const CurveTrack &curveTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }
          /**
           * @brief Returns the curve track.
           *
           * @return Reference to the curve track.
           */
          CurveTrack &curveTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }

    // midiCvTrack
    /**
     * @brief Returns the midi cv track.
     *
     * @return Reference to the midi cv track.
     */
    const MidiCvTrack &midiCvTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }
          /**
           * @brief Returns the midi cv track.
           *
           * @return Reference to the midi cv track.
           */
          MidiCvTrack &midiCvTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }

    // lfoTrack
    /**
     * @brief Returns the lfo track.
     *
     * @return Reference to the lfo track.
     */
    const LfoTrack &lfoTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Lfo); return *_track.lfo; }
          /**
           * @brief Returns the lfo track.
           *
           * @return Reference to the lfo track.
           */
          LfoTrack &lfoTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Lfo); return *_track.lfo; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a track in a fully defined default state.
     *
     * @note The active track mode must be initialized before initContainer()
     *       selects and constructs the corresponding payload.
     */
    Track() {
        initContainer();
    }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();
    /**
     * @brief Clears pattern.
     *
     * @param[in] patternIndex Zero-based pattern index.
     */
    void clearPattern(int patternIndex);
    /**
     * @brief Copies pattern.
     *
     * @param[in] src Source object/buffer/value.
     * @param[in] dst Destination object/buffer/value.
     */
    void copyPattern(int src, int dst);
    /**
     * @brief Duplicates pattern.
     *
     * @param[in] patternIndex Zero-based pattern index.
     *
     * @return `true` if duplicate pattern; otherwise `false`.
     */
    bool duplicatePattern(int patternIndex);

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

    /**
     * @brief Copies the compatible track state from another track while preserving this track index.
     *
     * @param[in] other Track whose mode and contents are copied.
     *
     * @return Reference to this track after assignment.
     */
    /**
     * @brief Copies the compatible track state from another track while preserving this track index.
     *
     * @param[in] other Track whose mode and contents are copied.
     *
     * @return Reference to this track after assignment.
     */
    Track &operator=(const Track &other) {
        ASSERT(_trackMode == other._trackMode, "invalid track mode");
        _linkTrack = other._linkTrack;
        _container = other._container;
        setContainerTrackIndex(_trackIndex);
        return *this;
    }

private:
    /**
     * @brief Sets the track index.
     *
     * @param[in] trackIndex Zero-based track index.
     */
    void setTrackIndex(int trackIndex);
    /**
     * @brief Sets the container track index.
     *
     * @param[in] trackIndex Zero-based track index.
     */
    void setContainerTrackIndex(int trackIndex);

    // Note: always call through Project::setTrackMode
    /**
     * @brief Sets the track mode.
     *
     * @param[in] trackMode Track mode used to select model/engine behavior.
     */
    void setTrackMode(TrackMode trackMode) {
        trackMode = ModelUtils::clampedEnum(trackMode);
        if (trackMode != _trackMode) {
            _trackMode = trackMode;
            initContainer();
        }
    }

    /**
     * @brief Initializes container.
     */
    void initContainer();

    /**
     * @brief Zero-based track index; a negative/sentinel value represents no selection where applicable.
     */
    uint8_t _trackIndex = -1; ///< Zero-based track index; a negative/sentinel value represents no selection where applicable.
    TrackMode _trackMode = TrackMode::Default; ///< Active track mode controlling the behavior of `Track`.
    int8_t _linkTrack = -1; ///< Zero-based source track used for linked playback; `-1` disables track linking.

    Container<NoteTrack, CurveTrack, MidiCvTrack, LfoTrack> _container; ///< In-place variant storage that owns the active object without heap allocation.
    /**
     * @brief Pointer to note; `nullptr` denotes that no object/resource is assigned.
     */
    union {
        NoteTrack *note;
        CurveTrack *curve;
        MidiCvTrack *midiCv;
        LfoTrack *lfo;
    } _track;

    friend class Project;
    friend class ClipBoard;
};

#undef SANITIZE_TRACK_MODE
