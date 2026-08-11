/**
 * @file ClipBoard.cpp
 * @brief Implements the ClipBoard component used by the sequencer model.
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
#include "ClipBoard.h"

#include "Model.h"
#include "ModelUtils.h"

ClipBoard::PatternSequence::PatternSequence() = default;

ClipBoard::PatternSequence::~PatternSequence() {
    clear();
}

void ClipBoard::PatternSequence::store(const NoteSequence &sequence) {
    clear();
    new (&_data.note) NoteSequence(sequence);
    _trackMode = Track::TrackMode::Note;
}

void ClipBoard::PatternSequence::store(const CurveSequence &sequence) {
    clear();
    new (&_data.curve) CurveSequence(sequence);
    _trackMode = Track::TrackMode::Curve;
}

void ClipBoard::PatternSequence::storeEmpty(Track::TrackMode trackMode) {
    clear();
    _trackMode = trackMode;
}

void ClipBoard::PatternSequence::clear() {
    switch (_trackMode) {
    case Track::TrackMode::Note:
        _data.note.~NoteSequence();
        break;
    case Track::TrackMode::Curve:
        _data.curve.~CurveSequence();
        break;
    case Track::TrackMode::MidiCv:
    case Track::TrackMode::Lfo:
    case Track::TrackMode::Last:
        break;
    }
    _trackMode = Track::TrackMode::Last;
}

ClipBoard::ClipBoard(Project &project) :
    _project(project)
{}

ClipBoard::~ClipBoard() {
    destroyValue();
}

void ClipBoard::clear() {
    destroyValue();
}

void ClipBoard::destroyValue() {
    switch (_type) {
    case Type::Track:
        _container.destroy(&_container.as<Track>());
        break;
    case Type::NoteSequence:
        _container.destroy(&_container.as<NoteSequence>());
        break;
    case Type::NoteSequenceSteps:
        _container.destroy(&_container.as<NoteSequenceSteps>());
        break;
    case Type::CurveSequence:
        _container.destroy(&_container.as<CurveSequence>());
        break;
    case Type::CurveSequenceSteps:
        _container.destroy(&_container.as<CurveSequenceSteps>());
        break;
    case Type::Pattern:
        _container.destroy(&_container.as<Pattern>());
        break;
    case Type::UserScale:
        _container.destroy(&_container.as<UserScale>());
        break;
    case Type::None:
        break;
    }
    _type = Type::None;
}

void ClipBoard::copyTrack(const Track &track) {
    auto &copy = createValue<Track>(Type::Track);
    copy.setTrackMode(track.trackMode());
    copy = track;
}

void ClipBoard::copyNoteSequence(const NoteSequence &noteSequence) {
    createValue<NoteSequence>(Type::NoteSequence) = noteSequence;
}

void ClipBoard::copyNoteSequenceSteps(const NoteSequence &noteSequence, const SelectedSteps &selectedSteps) {
    auto &noteSequenceSteps = createValue<NoteSequenceSteps>(Type::NoteSequenceSteps);
    noteSequenceSteps.sequence = noteSequence;
    noteSequenceSteps.selected = selectedSteps;
}

void ClipBoard::copyCurveSequence(const CurveSequence &curveSequence) {
    createValue<CurveSequence>(Type::CurveSequence) = curveSequence;
}

void ClipBoard::copyCurveSequenceSteps(const CurveSequence &curveSequence, const SelectedSteps &selectedSteps) {
    auto &curveSequenceSteps = createValue<CurveSequenceSteps>(Type::CurveSequenceSteps);
    curveSequenceSteps.sequence = curveSequence;
    curveSequenceSteps.selected = selectedSteps;
}

void ClipBoard::copyPattern(int patternIndex) {
    auto &pattern = createValue<Pattern>(Type::Pattern);
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &track = _project.track(trackIndex);
        auto &sequence = pattern.sequences[trackIndex];
        switch (track.trackMode()) {
        case Track::TrackMode::Note:
            sequence.store(track.noteTrack().sequence(patternIndex));
            break;
        case Track::TrackMode::Curve:
            sequence.store(track.curveTrack().sequence(patternIndex));
            break;
        case Track::TrackMode::MidiCv:
        case Track::TrackMode::Lfo:
        case Track::TrackMode::Last:
            sequence.storeEmpty(track.trackMode());
            break;
        }
    }
}

void ClipBoard::copyUserScale(const UserScale &userScale) {
    createValue<UserScale>(Type::UserScale) = userScale;
}

void ClipBoard::pasteTrack(Track &track) const {
    if (canPasteTrack()) {
        Model::ConfigLock lock;
        _project.setTrackMode(track.trackIndex(), _container.as<Track>().trackMode());
        track = _container.as<Track>();
    }
}

void ClipBoard::pasteNoteSequence(NoteSequence &noteSequence) const {
    if (canPasteNoteSequence()) {
        Model::WriteLock lock;
        noteSequence = _container.as<NoteSequence>();
    }
}

void ClipBoard::pasteNoteSequenceSteps(NoteSequence &noteSequence, const SelectedSteps &selectedSteps) const {
    if (canPasteNoteSequenceSteps()) {
        const auto &noteSequenceSteps = _container.as<NoteSequenceSteps>();
        ModelUtils::copySteps(noteSequenceSteps.sequence.steps(), noteSequenceSteps.selected, noteSequence.steps(), selectedSteps);
    }
}

void ClipBoard::pasteCurveSequence(CurveSequence &curveSequence) const {
    if (canPasteCurveSequence()) {
        Model::WriteLock lock;
        curveSequence = _container.as<CurveSequence>();
    }
}

void ClipBoard::pasteCurveSequenceSteps(CurveSequence &curveSequence, const SelectedSteps &selectedSteps) const {
    if (canPasteCurveSequenceSteps()) {
        const auto &curveSequenceSteps = _container.as<CurveSequenceSteps>();
        ModelUtils::copySteps(curveSequenceSteps.sequence.steps(), curveSequenceSteps.selected, curveSequence.steps(), selectedSteps);
    }
}

void ClipBoard::pastePattern(int patternIndex) const {
    if (canPastePattern()) {
        Model::WriteLock lock;
        const auto &pattern = _container.as<Pattern>();
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &track = _project.track(trackIndex);
            const auto &sequence = pattern.sequences[trackIndex];
            if (track.trackMode() == sequence.trackMode()) {
                switch (track.trackMode()) {
                case Track::TrackMode::Note:
                    track.noteTrack().sequence(patternIndex) = sequence.note();
                    break;
                case Track::TrackMode::Curve:
                    track.curveTrack().sequence(patternIndex) = sequence.curve();
                    break;
                case Track::TrackMode::MidiCv:
                case Track::TrackMode::Lfo:
                case Track::TrackMode::Last:
                    break;
                }
            }
        }
    }
}

void ClipBoard::pasteUserScale(UserScale &userScale) const {
    if (canPasteUserScale()) {
        userScale = _container.as<UserScale>();
    }
}

bool ClipBoard::canPasteTrack() const {
    return _type == Type::Track;
}

bool ClipBoard::canPasteNoteSequence() const {
    return _type == Type::NoteSequence;
}

bool ClipBoard::canPasteNoteSequenceSteps() const {
    return _type == Type::NoteSequenceSteps;
}

bool ClipBoard::canPasteCurveSequence() const {
    return _type == Type::CurveSequence;
}

bool ClipBoard::canPasteCurveSequenceSteps() const {
    return _type == Type::CurveSequenceSteps;
}

bool ClipBoard::canPastePattern() const {
    return _type == Type::Pattern;
}

bool ClipBoard::canPasteUserScale() const {
    return _type == Type::UserScale;
}
