/**
 * @file Project.h
 * @brief Declares the Project component used by the sequencer model.
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
#include "Observable.h"
#include "Types.h"
#include "TimeSignature.h"
#include "ClockSetup.h"
#include "Track.h"
#include "Song.h"
#include "PlayState.h"
#include "UserScale.h"
#include "Routing.h"
#include "MidiOutput.h"
#include "Serialize.h"
#include "FileDefs.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"
#include "core/utils/StringUtils.h"

/**
 * @brief Stores and manipulates project model data.
 */
class Project {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Name length constant used by this component.
     */
    static constexpr size_t NameLength = FileHeader::NameLength; ///< Maximum project-name length inherited from the fixed-width serialized file-header field.

    typedef std::array<Track, CONFIG_TRACK_COUNT> TrackArray;
    typedef std::array<uint8_t, CONFIG_CHANNEL_COUNT> CvOutputTrackArray;
    typedef std::array<uint8_t, CONFIG_CHANNEL_COUNT> GateOutputArray;

    /**
     * @brief Constructs a Project instance.
     */
    Project();

    //----------------------------------------
    // Properties
    //----------------------------------------

    // slot

    /**
     * @brief Returns the slot.
     *
     * @return Current project storage slot.
     */
    int slot() const { return _slot; }
    /**
     * @brief Sets the slot.
     *
     * @param[in] slot New zero-based project/storage slot index to store or apply.
     */
    void setSlot(int slot) {
        _slot = slot;
    }
    /**
     * @brief Returns the slot assigned.
     *
     * @return `true` if slot assigned; otherwise `false`.
     */
    bool slotAssigned() const {
        return _slot != uint8_t(-1);
    }

    // name

    /**
     * @brief Returns the name.
     *
     * @return Pointer to the name; `nullptr` when no value is available.
     */
    const char *name() const { return _name; }
    /**
     * @brief Sets the name.
     *
     * @param[in] name Null-terminated name or label consumed by the operation.
     */
    void setName(const char *name) {
        StringUtils::copy(_name, name, sizeof(_name));
    }

    // autoLoaded

    /**
     * @brief Returns the auto loaded.
     *
     * @return `true` if auto loaded; otherwise `false`.
     */
    bool autoLoaded() const { return _autoLoaded != 0; }
    /**
     * @brief Sets the auto loaded.
     *
     * @param[in] autoLoaded `true` to enable/set auto loaded; `false` to disable/clear it.
     */
    void setAutoLoaded(bool autoLoaded) { _autoLoaded = autoLoaded ? 1 : 0; }

    // tempo

    /**
     * @brief Returns the tempo.
     *
     * @return Project tempo in beats per minute.
     */
    float tempo() const { return _tempo.get(isRouted(Routing::Target::Tempo)); }
    /**
     * @brief Sets the tempo.
     *
     * @param[in] tempo New tempo in beats per minute to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setTempo(float tempo, bool routed = false) {
        _tempo.set(clamp(tempo, 1.f, 1000.f), routed);
    }

    /**
     * @brief Adjusts the tempo from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editTempo(int value, bool shift) {
        if (!isRouted(Routing::Target::Tempo)) {
            setTempo(tempo() + value * (shift ? 0.1f : 1.f));
        }
    }

    /**
     * @brief Formats the tempo into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printTempo(StringBuilder &str) const {
        printRouted(str, Routing::Target::Tempo);
        str(TXT_MODEL_TEMPO, tempo());
    }

    // swing

    /**
     * @brief Returns the swing.
     *
     * @return Configured swing amount in the model-defined percentage/range.
     */
    int swing() const { return _swing.get(isRouted(Routing::Target::Swing)); }
    /**
     * @brief Sets the swing.
     *
     * @param[in] swing Swing setting in the model-defined range.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setSwing(int swing, bool routed = false) {
        _swing.set(clamp(swing, 50, 75), routed);
    }

    /**
     * @brief Adjusts the swing from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editSwing(int value, bool shift) {
        if (!isRouted(Routing::Target::Swing)) {
            setSwing(ModelUtils::adjustedByStep(swing(), value, 5, !shift));
        }
    }

    /**
     * @brief Formats the swing into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printSwing(StringBuilder &str) const {
        printRouted(str, Routing::Target::Swing);
        str(TXT_MODEL_SWING, swing());
    }

    // timeSignature

    /**
     * @brief Returns the time signature.
     *
     * @return Configured project time signature.
     */
    TimeSignature timeSignature() const { return _timeSignature; }
    /**
     * @brief Sets the time signature.
     *
     * @param[in] timeSignature Time signature consumed by `setTimeSignature()`.
     */
    void setTimeSignature(TimeSignature timeSignature) {
        _timeSignature = timeSignature;
    }

    /**
     * @brief Adjusts the time signature from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editTimeSignature(int value, bool shift) {
        _timeSignature.edit(value, shift);
    }

    /**
     * @brief Formats the time signature into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printTimeSignature(StringBuilder &str) const {
        _timeSignature.print(str);
    }

    // syncMeasure

    /**
     * @brief Synchronizes measure.
     *
     * @return Configured synchronization-measure length.
     */
    int syncMeasure() const { return _syncMeasure; }
    /**
     * @brief Sets the sync measure.
     *
     * @param[in] syncMeasure Sync measure consumed by `setSyncMeasure()`.
     */
    void setSyncMeasure(int syncMeasure) {
        _syncMeasure = clamp(syncMeasure, 1, 128);
    }

    /**
     * @brief Adjusts the sync measure from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editSyncMeasure(int value, bool shift) {
        setSyncMeasure(ModelUtils::adjustedByPowerOfTwo(syncMeasure(), value, shift));
    }

    /**
     * @brief Formats the sync measure into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printSyncMeasure(StringBuilder &str) const {
        str(TXT_MODEL_PRINT_BAR_COUNT, syncMeasure(), syncMeasure() > 1 ? TXT_MODEL_PRINT_BAR_PLURAL : TXT_MODEL_PRINT_BAR);
    }

    // scale

    /**
     * @brief Returns the scale.
     *
     * @return Configured scale selection.
     */
    int scale() const { return _scale; }
    /**
     * @brief Sets the scale.
     *
     * @param[in] scale Scale definition used for quantization or display.
     */
    void setScale(int scale) {
        _scale = clamp(scale, 0, Scale::Count - 1);
    }

    /**
     * @brief Adjusts the scale from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editScale(int value, bool shift) {
#ifdef FIX_BROKEN_SCALE_CHANGE
        int newScale = clamp(scale() + value, 0, Scale::Count - 1);
        remapDefaultScaleChange(newScale);
#else
        setScale(scale() + value);
#endif
    }

    /**
     * @brief Formats the scale into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printScale(StringBuilder &str) const {
        str(Scale::name(scale()));
    }

    /**
     * @brief Selects ed scale.
     *
     * @return Reference to the selected scale.
     */
    const Scale &selectedScale() const {
        return Scale::get(scale());
    }

    // rootNote

    /**
     * @brief Returns the root note.
     *
     * @return Configured root-note selection.
     */
    int rootNote() const { return _rootNote; }
    /**
     * @brief Sets the root note.
     *
     * @param[in] rootNote New chromatic root-note index to store or apply.
     */
    void setRootNote(int rootNote) {
        _rootNote = clamp(rootNote, 0, 11);
    }

    /**
     * @brief Adjusts the root note from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRootNote(int value, bool shift) {
        setRootNote(rootNote() + value);
    }

    /**
     * @brief Formats the root note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRootNote(StringBuilder &str) const {
        Types::printNote(str, _rootNote);
    }

    // monitorMode

    /**
     * @brief Returns the monitor mode.
     *
     * @return Configured monitor mode.
     */
    Types::MonitorMode monitorMode() const { return _monitorMode; }
    /**
     * @brief Sets the monitor mode.
     *
     * @param[in] monitorMode Monitoring policy used while recording or auditioning the track.
     */
    void setMonitorMode(Types::MonitorMode monitorMode) {
        _monitorMode = ModelUtils::clampedEnum(monitorMode);
    }

    /**
     * @brief Adjusts the monitor mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMonitorMode(int value, bool shift) {
        _monitorMode = ModelUtils::adjustedEnum(_monitorMode, value);
    }

    /**
     * @brief Formats the monitor mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMonitorMode(StringBuilder &str) const {
        str(Types::monitorModeName(_monitorMode));
    }

    // recordMode

    /**
     * @brief Returns the record mode.
     *
     * @return Configured record mode.
     */
    Types::RecordMode recordMode() const { return _recordMode; }
    /**
     * @brief Sets the record mode.
     *
     * @param[in] recordMode Recording mode used to place incoming events.
     */
    void setRecordMode(Types::RecordMode recordMode) {
        _recordMode = ModelUtils::clampedEnum(recordMode);
    }

    /**
     * @brief Adjusts the record mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRecordMode(int value, bool shift) {
        _recordMode = ModelUtils::adjustedEnum(_recordMode, value);
    }

    /**
     * @brief Formats the record mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRecordMode(StringBuilder &str) const {
        str(Types::recordModeName(_recordMode));
    }

    // midiInput

    /**
     * @brief Adjusts the midi input from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMidiInput(int value, bool shift) {
        if (_midiInputMode == Types::MidiInputMode::Source) {
            if (value < 0 && _midiInputSource.isFirst()) {
                _midiInputMode = ModelUtils::adjustedEnum(_midiInputMode, value);
            } else {
                _midiInputSource.edit(value, shift);
            }
        } else {
            _midiInputMode = ModelUtils::adjustedEnum(_midiInputMode, value);
        }
    }

    /**
     * @brief Formats the midi input into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMidiInput(StringBuilder &str) const {
        switch (_midiInputMode) {
        case Types::MidiInputMode::Off:     str(TXT_MODEL_PRINT_OFF); break;
        case Types::MidiInputMode::All:     str(TXT_MODEL_PRINT_ALL); break;
        case Types::MidiInputMode::Source:  _midiInputSource.print(str); break;
        case Types::MidiInputMode::Last:    break;
        }
    }

    /**
     * @brief Returns the midi input mode.
     *
     * @return Configured midi input mode.
     */
    Types::MidiInputMode midiInputMode() const { return _midiInputMode; }
    /**
     * @brief Sets the midi input mode.
     *
     * @param[in] midiInputMode Midi input mode consumed by `setMidiInputMode()`.
     */
    void setMidiInputMode(Types::MidiInputMode midiInputMode) {
        _midiInputMode = ModelUtils::clampedEnum(midiInputMode);
    }

    /**
     * @brief Returns the midi input source.
     *
     * @return Reference to the midi input source.
     */
    const MidiSourceConfig &midiInputSource() const { return _midiInputSource; }
          /**
           * @brief Returns the midi input source.
           *
           * @return Reference to the midi input source.
           */
          MidiSourceConfig &midiInputSource()       { return _midiInputSource; }

    // cvGateInput

    /**
     * @brief Returns the cv gate input.
     *
     * @return Configured CV/gate input assignment.
     */
    Types::CvGateInput cvGateInput() const { return _cvGateInput; }
    /**
     * @brief Sets the cv gate input.
     *
     * @param[in] cvGateInput Configured CV/gate input mapping used by the track.
     */
    void setCvGateInput(Types::CvGateInput cvGateInput) {
        _cvGateInput = ModelUtils::clampedEnum(cvGateInput);
    }

    /**
     * @brief Adjusts the cv gate input from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editCvGateInput(int value, bool shift) {
        _cvGateInput = ModelUtils::adjustedEnum(_cvGateInput, value);
    }

    /**
     * @brief Formats the cv gate input into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printCvGateInput(StringBuilder &str) const {
        str(Types::cvGateInputName(_cvGateInput));
    }

    // curveCvInput

    /**
     * @brief Returns the curve cv input.
     *
     * @return Configured curve CV input assignment.
     */
    Types::CurveCvInput curveCvInput() const { return _curveCvInput; }
    /**
     * @brief Sets the curve cv input.
     *
     * @param[in] curveCvInput CV input mapping used while recording curve data.
     */
    void setCurveCvInput(Types::CurveCvInput curveCvInput) {
        _curveCvInput = ModelUtils::clampedEnum(curveCvInput);
    }

    /**
     * @brief Adjusts the curve cv input from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editCurveCvInput(int value, bool shift) {
        _curveCvInput = ModelUtils::adjustedEnum(_curveCvInput, value);
    }

    /**
     * @brief Formats the curve cv input into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printCurveCvInput(StringBuilder &str) const {
        str(Types::curveCvInput(_curveCvInput));
    }

    // curveMidiInput

    // clockSetup

    /**
     * @brief Returns the clock setup.
     *
     * @return Reference to the clock setup.
     */
    const ClockSetup &clockSetup() const { return _clockSetup; }
          /**
           * @brief Returns the clock setup.
           *
           * @return Reference to the clock setup.
           */
          ClockSetup &clockSetup()       { return _clockSetup; }

    // tracks

    /**
     * @brief Returns the tracks.
     *
     * @return Reference to the tracks.
     */
    const TrackArray &tracks() const { return _tracks; }
          /**
           * @brief Returns the tracks.
           *
           * @return Reference to the tracks.
           */
          TrackArray &tracks()       { return _tracks; }

    /**
     * @brief Returns track.
     *
     * @param[in] index Zero-based track index.
     *
     * @return Reference to the track.
     */
    const Track &track(int index) const { return _tracks[index]; }
          /**
           * @brief Returns track.
           *
           * @param[in] index Zero-based track index.
           *
           * @return Reference to the track.
           */
          Track &track(int index)       { return _tracks[index]; }

    // cvOutputTrack

    /**
     * @brief Returns the cv output tracks.
     *
     * @return Reference to the cv output tracks.
     */
    const CvOutputTrackArray &cvOutputTracks() const { return _cvOutputTracks; }
          /**
           * @brief Returns the cv output tracks.
           *
           * @return Reference to the cv output tracks.
           */
          CvOutputTrackArray &cvOutputTracks()       { return _cvOutputTracks; }

    /**
     * @brief Returns cv output track.
     *
     * @param[in] index Zero-based cv track index.
     *
     * @return Result of cvOutputTrack().
     */
    int cvOutputTrack(int index) const { return _cvOutputTracks[index]; }
    /**
     * @brief Sets the cv output track.
     *
     * @param[in] index Zero-based cv track index.
     * @param[in] trackIndex Zero-based track index.
     */
    void setCvOutputTrack(int index, int trackIndex) { _cvOutputTracks[index] = clamp(trackIndex, 0, CONFIG_TRACK_COUNT - 1); }

    /**
     * @brief Adjusts the cv output track from a UI edit delta.
     *
     * @param[in] index Zero-based cv track index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editCvOutputTrack(int index, int value, bool shift) {
        setCvOutputTrack(index, cvOutputTrack(index) + value);
    }

    // gateOutputTrack

    /**
     * @brief Returns the gate output tracks.
     *
     * @return Reference to the gate output tracks.
     */
    const GateOutputArray &gateOutputTracks() const { return _gateOutputTracks; }
          /**
           * @brief Returns the gate output tracks.
           *
           * @return Reference to the gate output tracks.
           */
          GateOutputArray &gateOutputTracks()       { return _gateOutputTracks; }

    /**
     * @brief Returns gate output track.
     *
     * @param[in] index Zero-based gate track index.
     *
     * @return Result of gateOutputTrack().
     */
    int gateOutputTrack(int index) const { return _gateOutputTracks[index]; }
    /**
     * @brief Sets the gate output track.
     *
     * @param[in] index Zero-based gate track index.
     * @param[in] trackIndex Zero-based track index.
     */
    void setGateOutputTrack(int index, int trackIndex) { _gateOutputTracks[index] = clamp(trackIndex, 0, CONFIG_TRACK_COUNT - 1); }

    /**
     * @brief Adjusts the gate output track from a UI edit delta.
     *
     * @param[in] index Zero-based gate track index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editGateOutputTrack(int index, int value, bool shift) {
        setGateOutputTrack(index, gateOutputTrack(index) + value);
    }

    // song

    /**
     * @brief Returns the song.
     *
     * @return Reference to the song.
     */
    const Song &song() const { return _song; }
          /**
           * @brief Returns the song.
           *
           * @return Reference to the song.
           */
          Song &song()       { return _song; }

    // playState

    /**
     * @brief Returns the play state.
     *
     * @return Reference to the play state.
     */
    const PlayState &playState() const { return _playState; }
          /**
           * @brief Returns the play state.
           *
           * @return Reference to the play state.
           */
          PlayState &playState()       { return _playState; }

    // userScales

    /**
     * @brief Returns the user scales.
     *
     * @return Reference to the user scales.
     */
    const UserScale::Array &userScales() const { return UserScale::userScales; }
          /**
           * @brief Returns the user scales.
           *
           * @return Reference to the user scales.
           */
          UserScale::Array &userScales()       { return UserScale::userScales; }

    /**
     * @brief Returns user scale.
     *
     * @param[in] index Zero-based user scale index.
     *
     * @return Reference to the user scale.
     */
    const UserScale &userScale(int index) const { return UserScale::userScales[index]; }
          /**
           * @brief Returns user scale.
           *
           * @param[in] index Zero-based user scale index.
           *
           * @return Reference to the user scale.
           */
          UserScale &userScale(int index)       { return UserScale::userScales[index]; }

    // routing

    /**
     * @brief Returns the routing.
     *
     * @return Reference to the routing.
     */
    const Routing &routing() const { return _routing; }
          /**
           * @brief Returns the routing.
           *
           * @return Reference to the routing.
           */
          Routing &routing()       { return _routing; }

    // midiOutput

    /**
     * @brief Returns the midi output.
     *
     * @return Reference to the midi output.
     */
    const MidiOutput &midiOutput() const { return _midiOutput; }
          /**
           * @brief Returns the midi output.
           *
           * @return Reference to the midi output.
           */
          MidiOutput &midiOutput()       { return _midiOutput; }

    // selectedTrackIndex

    /**
     * @brief Selects ed track index.
     *
     * @return Zero-based selected track index.
     */
    int selectedTrackIndex() const { return _selectedTrackIndex; }
    /**
     * @brief Sets the selected track index.
     *
     * @param[in] index Zero-based selected track index index.
     */
    void setSelectedTrackIndex(int index) {
        index = clamp(index, 0, CONFIG_TRACK_COUNT - 1);
        if (index != _selectedTrackIndex) {
            _selectedTrackIndex = index;
            _observable.notify(SelectedTrackIndexChanged);

            // switch selected pattern
            setSelectedPatternIndex(_playState.trackState(index).pattern());
        }
    }

    /**
     * @brief Reports whether selected track.
     *
     * @param[in] index Zero-based selected track index.
     *
     * @return `true` if selected track; otherwise `false`.
     */
    bool isSelectedTrack(int index) const { return _selectedTrackIndex == index; }

    // selectedPatternIndex

    /**
     * @brief Selects ed pattern index.
     *
     * @return Zero-based selected pattern index.
     */
    int selectedPatternIndex() const {
        return _playState.snapshotActive() ? PlayState::SnapshotPatternIndex : _selectedPatternIndex;
    }

    /**
     * @brief Sets the selected pattern index.
     *
     * @param[in] index Zero-based selected pattern index index.
     */
    void setSelectedPatternIndex(int index) {
        _selectedPatternIndex = clamp(index, 0, CONFIG_PATTERN_COUNT - 1);
        _observable.notify(SelectedPatternIndexChanged);
    }

    /**
     * @brief Reports whether selected pattern.
     *
     * @param[in] index Zero-based selected pattern index.
     *
     * @return `true` if selected pattern; otherwise `false`.
     */
    bool isSelectedPattern(int index) const { return _selectedPatternIndex == index; }

    /**
     * @brief Adjusts the selected pattern index from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editSelectedPatternIndex(int value, bool shift) {
        setSelectedPatternIndex(selectedPatternIndex() + value);
    }

    // selectedNoteSequenceLayer

    /**
     * @brief Selects ed note sequence layer.
     *
     * @return Currently selected note-sequence edit layer.
     */
    NoteSequence::Layer selectedNoteSequenceLayer() const { return _selectedNoteSequenceLayer; }
    /**
     * @brief Sets the selected note sequence layer.
     *
     * @param[in] layer Layer index or layer identifier.
     */
    void setSelectedNoteSequenceLayer(NoteSequence::Layer layer) { _selectedNoteSequenceLayer = layer; }

    // selectedCurveSequenceLayer

    /**
     * @brief Selects ed curve sequence layer.
     *
     * @return Currently selected curve-sequence edit layer.
     */
    CurveSequence::Layer selectedCurveSequenceLayer() const { return _selectedCurveSequenceLayer; }
    /**
     * @brief Sets the selected curve sequence layer.
     *
     * @param[in] layer Layer index or layer identifier.
     */
    void setSelectedCurveSequenceLayer(CurveSequence::Layer layer) { _selectedCurveSequenceLayer = layer; }

    // selectedTrack

    /**
     * @brief Selects ed track.
     *
     * @return Reference to the selected track.
     */
    const Track &selectedTrack() const { return _tracks[_selectedTrackIndex]; }
          /**
           * @brief Selects ed track.
           *
           * @return Reference to the selected track.
           */
          Track &selectedTrack()       { return _tracks[_selectedTrackIndex]; }

    // noteSequence

    /**
     * @brief Returns note sequence.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] patternIndex Zero-based pattern index.
     *
     * @return Reference to the note sequence.
     */
    const NoteSequence &noteSequence(int trackIndex, int patternIndex) const { return _tracks[trackIndex].noteTrack().sequence(patternIndex); }
          /**
           * @brief Returns note sequence.
           *
           * @param[in] trackIndex Zero-based track index.
           * @param[in] patternIndex Zero-based pattern index.
           *
           * @return Reference to the note sequence.
           */
          NoteSequence &noteSequence(int trackIndex, int patternIndex)       { return _tracks[trackIndex].noteTrack().sequence(patternIndex); }

    // selectedNoteSequence

    /**
     * @brief Selects ed note sequence.
     *
     * @return Reference to the selected note sequence.
     */
    const NoteSequence &selectedNoteSequence() const { return noteSequence(_selectedTrackIndex, selectedPatternIndex()); }
          /**
           * @brief Selects ed note sequence.
           *
           * @return Reference to the selected note sequence.
           */
          NoteSequence &selectedNoteSequence()       { return noteSequence(_selectedTrackIndex, selectedPatternIndex()); }

    // curveSequence

    /**
     * @brief Returns curve sequence.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] patternIndex Zero-based pattern index.
     *
     * @return Reference to the curve sequence.
     */
    const CurveSequence &curveSequence(int trackIndex, int patternIndex) const { return _tracks[trackIndex].curveTrack().sequence(patternIndex); }
          /**
           * @brief Returns curve sequence.
           *
           * @param[in] trackIndex Zero-based track index.
           * @param[in] patternIndex Zero-based pattern index.
           *
           * @return Reference to the curve sequence.
           */
          CurveSequence &curveSequence(int trackIndex, int patternIndex)       { return _tracks[trackIndex].curveTrack().sequence(patternIndex); }

    // selectedCurveSequence

    /**
     * @brief Selects ed curve sequence.
     *
     * @return Reference to the selected curve sequence.
     */
    const CurveSequence &selectedCurveSequence() const { return curveSequence(_selectedTrackIndex, selectedPatternIndex()); }
          /**
           * @brief Selects ed curve sequence.
           *
           * @return Reference to the selected curve sequence.
           */
          CurveSequence &selectedCurveSequence()       { return curveSequence(_selectedTrackIndex, selectedPatternIndex()); }

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
    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target); }
    /**
     * @brief Formats the routed into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] target Target object/value affected by the operation.
     */
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target); }
    /**
     * @brief Writes routed.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] intValue Integer representation of the value.
     * @param[in] floatValue Floating-point representation of the value.
     */
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Observable
    //----------------------------------------

    /**
     * @brief Enumerates the supported event values.
     */
    enum Event {
        ProjectCleared, ///< Selects the project cleared event.
        ProjectRead, ///< Selects the project read event.
        TrackModeChanged, ///< Selects the track mode changed event.
        SelectedTrackIndexChanged, ///< Selects the selected track index changed event.
        SelectedPatternIndexChanged, ///< Selects the selected pattern index changed event.
    };

    /**
     * @brief Registers or evaluates the watched value.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void watch(std::function<void(Event)> handler) {
        _observable.watch(handler);
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

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
     * @brief Sets the track mode.
     *
     * @param[in] trackIndex Zero-based track index.
     * @param[in] trackMode Track mode used to select model/engine behavior.
     */
    void setTrackMode(int trackIndex, Track::TrackMode trackMode);

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     *
     * @return `true` if read; otherwise `false`.
     */
    bool read(VersionedSerializedReader &reader);

#ifdef FIX_BROKEN_SCALE_CHANGE
    /**
     * @brief Remaps note data after the project default scale changes.
     *
     * @param[in] newScale Replacement scale used when remapping note data.
     */
    void remapDefaultScaleChange(int newScale) {
        const Scale &oldScale = selectedScale();
        const Scale &newScaleRef = Scale::get(newScale);

        for (auto &track : _tracks) {
            // only remap sequences for track types that actually contain NoteSequence
            switch (track.trackMode()) {
                case Track::TrackMode::Note:
                    for (int pattern = 0; pattern < CONFIG_PATTERN_COUNT; ++pattern) {
                        auto &sequence = track.noteTrack().sequence(pattern);
                        if (sequence.scale() < 0) {
                            sequence.remapScale(oldScale, newScaleRef);
                        }
                    }
                    break;
                default:
                    // other track types do not contain NoteSequence; nothing to do
                    break;
            }
        }

        setScale(newScale);
    }
#endif

private:
    /**
     * @brief Returns the uint8 t.
     *
     * @return The uint8 t value.
     */
    uint8_t _slot = uint8_t(-1); ///< Assigned project storage slot; `UINT8_MAX` denotes an unsaved/unassigned project. ///< Assigned project storage slot; `UINT8_MAX` denotes an unsaved/unassigned project.
    char _name[NameLength + 1]; ///< Null-terminated user-visible project name with storage for `NameLength` characters plus the terminator.
    mutable uint8_t _autoLoaded = 0; ///< Whether the project/state was loaded automatically during startup.
    Routable<float> _tempo; ///< Base and routed tempo setting, resolved through the routing system when queried.
    Routable<uint8_t> _swing; ///< Base and routed swing setting, resolved through the routing system when queried.
    TimeSignature _timeSignature; ///< Configured project time signature.
    uint8_t _syncMeasure; ///< Measure interval used for synchronized project/pattern operations.
    uint8_t _scale; ///< Index of the project-wide default scale in the `Scale` registry.
    uint8_t _rootNote; ///< Project-wide default chromatic root note, encoded as 0..11.
    Types::RecordMode _recordMode; ///< Recording policy applied when incoming notes/CV are written into sequences.
    Types::MonitorMode _monitorMode; ///< Monitoring policy controlling how live input is forwarded while editing or recording.
    Types::MidiInputMode _midiInputMode; ///< MIDI-input interpretation mode selecting source/channel behavior for the project.
    MidiSourceConfig _midiInputSource; ///< MIDI routing/source configuration.
    Types::CvGateInput _cvGateInput; ///< Configured CV/gate input mapping used for live note recording and monitoring.
    Types::CurveCvInput _curveCvInput; ///< Configured CV input mapping used for curve recording and monitoring.

    ClockSetup _clockSetup; ///< Project clock configuration.
    TrackArray _tracks; ///< Persistent track models contained in the current project.
    CvOutputTrackArray _cvOutputTracks; ///< Persistent CV-output track assignments contained in the current project.
    GateOutputArray _gateOutputTracks; ///< Persistent gate-output track assignments contained in the current project.
    Song _song; ///< Song arrangement model.
    PlayState _playState; ///< Runtime project playback state.
    Routing _routing; ///< Project routing configuration.
    MidiOutput _midiOutput; ///< Project MIDI-output configuration.

    /**
     * @brief Zero-based selected track index; a negative/sentinel value represents no selection where applicable.
     */
    int _selectedTrackIndex = 0; ///< Zero-based selected track index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Zero-based selected pattern index; a negative/sentinel value represents no selection where applicable.
     */
    int _selectedPatternIndex = 0; ///< Zero-based selected pattern index; a negative/sentinel value represents no selection where applicable.
    /**
     * @brief Returns the layer.
     *
     * @return The layer value.
     */
    NoteSequence::Layer _selectedNoteSequenceLayer = NoteSequence::Layer(0); ///< Note-sequence editing layer currently selected by the UI. ///< Note-sequence editing layer currently selected by the UI.
    /**
     * @brief Returns the layer.
     *
     * @return The layer value.
     */
    CurveSequence::Layer _selectedCurveSequenceLayer = CurveSequence::Layer(0); ///< Curve-sequence editing layer currently selected by the UI. ///< Curve-sequence editing layer currently selected by the UI.

    Observable<Event, 2> _observable; ///< Observer dispatcher used to publish project model changes.
};

