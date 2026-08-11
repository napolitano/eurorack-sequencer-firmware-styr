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

class Track {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class TrackMode : uint8_t {
        Note,
        Curve,
        MidiCv,
        Lfo,
        Last,
        Default = Note
    };

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

    int trackIndex() const { return _trackIndex; }

    // trackMode

    TrackMode trackMode() const { return _trackMode; }

    void printTrackMode(StringBuilder &str) const {
        str(trackModeName(trackMode()));
    }

    // linkTrack

    int linkTrack() const { return _linkTrack; }
    void setLinkTrack(int linkTrack) {
        _linkTrack = clamp(linkTrack, -1, _trackIndex - 1);
    }

    void editLinkTrack(int value, bool shift) {
        setLinkTrack(linkTrack() + value);
    }

    void printLinkTrack(StringBuilder &str) const {
        if (linkTrack() == -1) {
            str(TXT_LIST_LABEL_NONE);
        } else {
            str(TXT_LIST_LABEL_TRACK, linkTrack() + 1);
        }
    }

    // noteTrack
    const NoteTrack &noteTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }
          NoteTrack &noteTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Note); return *_track.note; }

    // curveTrack
    const CurveTrack &curveTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }
          CurveTrack &curveTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Curve); return *_track.curve; }

    // midiCvTrack
    const MidiCvTrack &midiCvTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }
          MidiCvTrack &midiCvTrack()       { SANITIZE_TRACK_MODE(_trackMode, TrackMode::MidiCv); return *_track.midiCv; }

    // lfoTrack
    const LfoTrack &lfoTrack() const { SANITIZE_TRACK_MODE(_trackMode, TrackMode::Lfo); return *_track.lfo; }
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

    void clear();
    void clearPattern(int patternIndex);
    void copyPattern(int src, int dst);
    bool duplicatePattern(int patternIndex);

    void gateOutputName(int index, StringBuilder &str) const;
    void cvOutputName(int index, StringBuilder &str) const;

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

    Track &operator=(const Track &other) {
        ASSERT(_trackMode == other._trackMode, "invalid track mode");
        _linkTrack = other._linkTrack;
        _container = other._container;
        setContainerTrackIndex(_trackIndex);
        return *this;
    }

private:
    void setTrackIndex(int trackIndex);
    void setContainerTrackIndex(int trackIndex);

    // Note: always call through Project::setTrackMode
    void setTrackMode(TrackMode trackMode) {
        trackMode = ModelUtils::clampedEnum(trackMode);
        if (trackMode != _trackMode) {
            _trackMode = trackMode;
            initContainer();
        }
    }

    void initContainer();

    uint8_t _trackIndex = -1;
    TrackMode _trackMode = TrackMode::Default;
    int8_t _linkTrack = -1;

    Container<NoteTrack, CurveTrack, MidiCvTrack, LfoTrack> _container;
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
