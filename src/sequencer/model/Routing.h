/**
 * @file Routing.h
 * @brief Declares the Routing component used by the sequencer model.
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
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <bitset>

#include <cstdint>

class Project;
class NoteSequence;
class CurveSequence;

/**
 * @brief Stores and manipulates routing model data.
 */
class Routing {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported target values.
     */
    enum class Target : uint8_t {
        None, ///< Represents no selection or an invalid value.
        // Engine targets
        EngineFirst, ///< Selects the engine first target.
        Play = EngineFirst, ///< Selects the play target.
        PlayToggle, ///< Selects the play toggle target.
        Record, ///< Selects the record target.
        RecordToggle, ///< Selects the record toggle target.
        TapTempo, ///< Selects the tap tempo target.
        EngineLast = TapTempo, ///< Selects the engine last target.

        // Project targets
        ProjectFirst, ///< Selects the project first target.
        Tempo = ProjectFirst, ///< Selects the tempo target.
        Swing, ///< Selects the swing target.
        ProjectLast = Swing, ///< Selects the project last target.

        // PlayState targets
        PlayStateFirst, ///< Selects the play state first target.
        Mute = PlayStateFirst, ///< Selects the mute target.
        Fill, ///< Selects the fill target.
        FillAmount, ///< Selects the fill amount target.
        Pattern, ///< Selects the pattern target.
        PlayStateLast = Pattern, ///< Selects the play state last target.

        // Track targets
        TrackFirst, ///< Selects the track first target.
        SlideTime = TrackFirst, ///< Selects the slide time target.
        Octave, ///< Selects the octave target.
        Transpose, ///< Selects the transpose target.
        Offset, ///< Selects the offset target.
        Rotate, ///< Selects the rotate target.
        GateProbabilityBias, ///< Selects the gate probability bias target.
        RetriggerProbabilityBias, ///< Selects the retrigger probability bias target.
        LengthBias, ///< Selects the length bias target.
        NoteProbabilityBias, ///< Selects the note probability bias target.
        ShapeProbabilityBias, ///< Selects the shape probability bias target.
        TrackLast = ShapeProbabilityBias, ///< Selects the track last target.

        // LFO targets
        LfoFirst, ///< Selects the lfo first target.
        LfoHi = LfoFirst, ///< Selects the lfo hi target.
        LfoLow, ///< Selects the lfo low target.
        LfoSpeed, ///< Selects the lfo speed target.
        LfoShape, ///< Selects the lfo shape target.
        LfoMode, ///< Selects the lfo mode target.
        LfoClip, ///< Selects the lfo clip target.
        LfoPulseWidth, ///< Selects the lfo pulse width target.
        LfoLast = LfoPulseWidth, ///< Selects the lfo last target.

        // Sequence targets
        SequenceFirst, ///< Selects the sequence first target.
        FirstStep = SequenceFirst, ///< Selects the first step target.
        LastStep, ///< Selects the last step target.
        RunMode, ///< Selects the run mode target.
        Divisor, ///< Selects the divisor target.
        Scale, ///< Selects the scale target.
        RootNote, ///< Selects the root note target.
        SequenceLast = RootNote, ///< Selects the sequence last target.

        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return Pointer to the target name; `nullptr` when no value is available.
     */
    static const char *targetName(Target target) {
        switch (target) {
        case Target::None:                      return TXT_LIST_LABEL_NONE;

        case Target::Play:                      return TXT_LIST_LABEL_PLAY;
        case Target::PlayToggle:                return TXT_LIST_LABEL_PLAY_TOGGLE;
        case Target::Record:                    return TXT_LIST_LABEL_RECORD;
        case Target::RecordToggle:              return TXT_LIST_LABEL_RECORD_TOGGLE;
        case Target::TapTempo:                  return TXT_LIST_LABEL_TAP_TEMPO;

        case Target::Tempo:                     return TXT_LIST_LABEL_TEMPO;
        case Target::Swing:                     return TXT_LIST_LABEL_SWING;

        case Target::Mute:                      return TXT_LIST_LABEL_MUTE;
        case Target::Fill:                      return TXT_LIST_LABEL_FILL;
        case Target::FillAmount:                return TXT_LIST_LABEL_FILL_AMOUNT;
        case Target::Pattern:                   return TXT_LIST_LABEL_PATTERN;

        case Target::SlideTime:                 return TXT_LIST_LABEL_SLIDE_TIME;
        case Target::Octave:                    return TXT_LIST_LABEL_OCTAVE;
        case Target::Transpose:                 return TXT_LIST_LABEL_TRANSPOSE;
        case Target::Offset:                    return TXT_LIST_LABEL_OFFSET;
        case Target::Rotate:                    return TXT_LIST_LABEL_ROTATE;
        case Target::GateProbabilityBias:       return TXT_LIST_LABEL_GATE_PROBABILITY_BIAS;
        case Target::RetriggerProbabilityBias:  return TXT_LIST_LABEL_GATE_RETRIGGER_PROBABILITY_BIAS;
        case Target::LengthBias:                return TXT_LIST_LABEL_GATE_LENGTH_BIAS;
        case Target::NoteProbabilityBias:       return TXT_LIST_LABEL_GATE_NOTE_PROBABILITY_BIAS;
        case Target::ShapeProbabilityBias:      return TXT_LIST_LABEL_GATE_SHAPE_PROBABILITY_BIAS;

            // LFO
            case Target::LfoHi:                     return "LFO Hi";
            case Target::LfoLow:                    return "LFO Low";
            case Target::LfoSpeed:                  return "LFO Speed";
            case Target::LfoShape:                  return "LFO Shape";
            case Target::LfoMode:                   return "LFO Mode";
            case Target::LfoClip:                   return "LFO Clip";
            case Target::LfoPulseWidth:             return "LFO PulseWidth";

        case Target::FirstStep:                 return TXT_LIST_LABEL_FIRST_STEP;
        case Target::LastStep:                  return TXT_LIST_LABEL_LAST_STEP;
        case Target::RunMode:                   return TXT_LIST_LABEL_RUN_MODE;
        case Target::Divisor:                   return TXT_LIST_LABEL_DIVISOR;
        case Target::Scale:                     return TXT_LIST_LABEL_SCALE;
        case Target::RootNote:                  return TXT_LIST_LABEL_ROOT_NOTE;

        case Target::Last:                      break;
        }
        return nullptr;
    }

    /**
     * @brief Returns target serialize.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return Result of targetSerialize().
     */
    static uint8_t targetSerialize(Target target) {
        switch (target) {
        case Target::None:                      return 0;
        case Target::Play:                      return 1;
        case Target::Record:                    return 2;
        case Target::Tempo:                     return 3;
        case Target::Swing:                     return 4;
        case Target::SlideTime:                 return 5;
        case Target::Octave:                    return 6;
        case Target::Transpose:                 return 7;
        case Target::Rotate:                    return 8;
        case Target::GateProbabilityBias:       return 9;
        case Target::RetriggerProbabilityBias:  return 10;
        case Target::LengthBias:                return 11;
        case Target::NoteProbabilityBias:       return 12;
        case Target::Divisor:                   return 13;
        case Target::RunMode:                   return 14;
        case Target::FirstStep:                 return 15;
        case Target::LastStep:                  return 16;

        case Target::Mute:                      return 17;
        case Target::Fill:                      return 18;
        case Target::FillAmount:                return 19;
        case Target::Pattern:                   return 20;

        case Target::TapTempo:                  return 21;

        case Target::ShapeProbabilityBias:      return 22;

        case Target::Scale:                     return 23;
        case Target::RootNote:                  return 24;

        case Target::Offset:                    return 25;

        case Target::PlayToggle:                return 26;
        case Target::RecordToggle:              return 27;

            // LFO
            case Target::LfoHi:                     return 28;
            case Target::LfoLow:                    return 29;
            case Target::LfoSpeed:                  return 30;
            case Target::LfoShape:                  return 31;
            case Target::LfoMode:                   return 32;
            case Target::LfoClip:                   return 33;
            case Target::LfoPulseWidth:             return 34;

        case Target::Last:                      break;
        }
        return 0;
    }

    /**
     * @brief Reports whether engine target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if engine target; otherwise `false`.
     */
    static bool isEngineTarget(Target target) {
        return target >= Target::EngineFirst && target <= Target::EngineLast;
    }

    /**
     * @brief Reports whether project target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if project target; otherwise `false`.
     */
    static bool isProjectTarget(Target target) {
        return target >= Target::ProjectFirst && target <= Target::ProjectLast;
    }

    /**
     * @brief Reports whether play state target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if play state target; otherwise `false`.
     */
    static bool isPlayStateTarget(Target target) {
        return target >= Target::PlayStateFirst && target <= Target::PlayStateLast;
    }

    /**
     * @brief Reports whether track target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if track target; otherwise `false`.
     */
    static bool isTrackTarget(Target target) {
        return (target >= Target::TrackFirst && target <= Target::TrackLast) ||
               (target >= Target::LfoFirst && target <= Target::LfoLast);
    }

    /**
     * @brief Reports whether sequence target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if sequence target; otherwise `false`.
     */
    static bool isSequenceTarget(Target target) {
        return target >= Target::SequenceFirst && target <= Target::SequenceLast;
    }

    /**
     * @brief Reports whether per track target.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return `true` if per track target; otherwise `false`.
     */
    static bool isPerTrackTarget(Target target) {
        return isPlayStateTarget(target) || isTrackTarget(target) || isSequenceTarget(target);
    }

    /**
     * @brief Enumerates the supported source values.
     */
    enum class Source : uint8_t {
        None, ///< Represents no selection or an invalid value.
        CvIn1, ///< Selects the cv in1 source.
        CvFirst = CvIn1, ///< Selects the cv first source.
        CvIn2, ///< Selects the cv in2 source.
        CvIn3, ///< Selects the cv in3 source.
        CvIn4, ///< Selects the cv in4 source.
        CvOut1, ///< Selects the cv out1 source.
        CvOut2, ///< Selects the cv out2 source.
        CvOut3, ///< Selects the cv out3 source.
        CvOut4, ///< Selects the cv out4 source.
        CvOut5, ///< Selects the cv out5 source.
        CvOut6, ///< Selects the cv out6 source.
        CvOut7, ///< Selects the cv out7 source.
        CvOut8, ///< Selects the cv out8 source.
        CvLast = CvOut8, ///< Selects the cv last source.
        Midi, ///< Selects the midi source.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Reports whether cv source.
     *
     * @param[in] source Routing source whose value or configuration is queried or changed.
     *
     * @return `true` if cv source; otherwise `false`.
     */
    static bool isCvSource(Source source) { return source >= Source::CvFirst && source <= Source::CvLast; }
    /**
     * @brief Reports whether midi source.
     *
     * @param[in] source Routing source whose value or configuration is queried or changed.
     *
     * @return `true` if midi source; otherwise `false`.
     */
    static bool isMidiSource(Source source) { return source == Source::Midi; }

    /**
     * @brief Formats the source into the supplied string builder/output.
     *
     * @param[in] source Routing source whose value or configuration is queried or changed.
     * @param[out] str String builder that receives the formatted representation.
     */
    static void printSource(Source source, StringBuilder &str) {
        switch (source) {
        case Source::None:
            str(TXT_LIST_LABEL_NONE);
            break;
        case Source::CvIn1:
        case Source::CvIn2:
        case Source::CvIn3:
        case Source::CvIn4:
            str(TXT_LIST_LABEL_CV_IN_VALUE, int(source) - int(Source::CvIn1) + 1);
            break;
        case Source::CvOut1:
        case Source::CvOut2:
        case Source::CvOut3:
        case Source::CvOut4:
        case Source::CvOut5:
        case Source::CvOut6:
        case Source::CvOut7:
        case Source::CvOut8:
            str(TXT_LIST_LABEL_CV_OUT_VALUE, int(source) - int(Source::CvOut1) + 1);
            break;
        case Source::Midi:
            str(TXT_LIST_LABEL_MIDI);
        case Source::Last:
            break;
        }
    }

    /**
     * @brief Stores and manipulates cv source model data.
     */
    class CvSource {
    public:
        // range

        /**
         * @brief Returns the range.
         *
         * @return Configured numeric/source range.
         */
        Types::VoltageRange range() const { return _range; }
        /**
         * @brief Sets the range.
         *
         * @param[in] range Inclusive/exclusive value span as defined by the owning API.
         */
        void setRange(Types::VoltageRange range) {
            _range = ModelUtils::clampedEnum(range);
        }

        /**
         * @brief Adjusts the range from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editRange(int value, bool shift) {
            setRange(ModelUtils::adjustedEnum(range(), value));
        }

        /**
         * @brief Formats the range into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printRange(StringBuilder &str) const {
            str(Types::voltageRangeName(range()));
        }

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

        /**
         * @brief Compares this value with another instance for value equality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state is equal; otherwise `false`.
         */
        bool operator==(const CvSource &other) const;

    private:
        Types::VoltageRange _range; ///< Configured CV voltage range.
    };

    /**
     * @brief Stores and manipulates midi source model data.
     */
    class MidiSource {
    public:
        /**
         * @brief Enumerates the supported event values.
         */
        enum class Event : uint8_t {
            ControlAbsolute, ///< Selects the control absolute event.
            ControlRelative, ///< Selects the control relative event.
            LastControlEvent = ControlRelative, ///< Selects the last control event event.
            PitchBend, ///< Selects the pitch bend event.
            NoteMomentary, ///< Selects the note momentary event.
            NoteToggle, ///< Selects the note toggle event.
            NoteVelocity, ///< Selects the note velocity event.
            NoteRange, ///< Selects the note range event.
            Last, ///< Sentinel marking the end of the valid enumeration range.
        };

        /**
         * @brief Returns the display name for event.
         *
         * @param[in] event Event to process.
         *
         * @return Pointer to the event name; `nullptr` when no value is available.
         */
        static const char *eventName(Event event) {
            switch (event) {
            case Event::ControlAbsolute:return TXT_LIST_LABEL_CC_ABSOLUTE;
            case Event::ControlRelative:return TXT_LIST_LABEL_CC_RELATIVE;
            case Event::PitchBend:      return TXT_LIST_LABEL_PITCH_BEND;
            case Event::NoteMomentary:  return TXT_LIST_LABEL_NOTE_MOMENTARY;
            case Event::NoteToggle:     return TXT_LIST_LABEL_NOTE_TOGGLE;
            case Event::NoteVelocity:   return TXT_LIST_LABEL_NOTE_VELOCITY;
            case Event::NoteRange:      return TXT_LIST_LABEL_NOTE_RANGE;
            case Event::Last:           break;
            }
            return nullptr;
        }

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

        // event

        /**
         * @brief Returns the event.
         *
         * @return Configured event/source selection.
         */
        Event event() const { return _event; }
        /**
         * @brief Sets the event.
         *
         * @param[in] event Event to process.
         */
        void setEvent(Event event) {
            _event = ModelUtils::clampedEnum(event);
        }

        /**
         * @brief Adjusts the event from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editEvent(int value, bool shift) {
            setEvent(ModelUtils::adjustedEnum(event(), value));
        }

        /**
         * @brief Formats the event into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printEvent(StringBuilder &str) const {
            str(eventName(event()));
        }

        /**
         * @brief Reports whether control event.
         *
         * @return `true` if control event; otherwise `false`.
         */
        bool isControlEvent() const {
            return int(_event) <= int(Event::LastControlEvent);
        }

        // controlNumber

        /**
         * @brief Returns the control number.
         *
         * @return MIDI Control Change controller number in the 0..127 range.
         */
        int controlNumber() const { return _controlNumberOrNote; }
        /**
         * @brief Sets the control number.
         *
         * @param[in] controlNumber New MIDI Control Change controller number in the 0..127 range to store or apply.
         */
        void setControlNumber(int controlNumber) {
            _controlNumberOrNote = clamp(controlNumber, 0, 127);
        }

        /**
         * @brief Adjusts the control number from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editControlNumber(int value, bool shift) {
            setControlNumber(controlNumber() + value);
        }

        /**
         * @brief Formats the control number into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printControlNumber(StringBuilder &str) const {
            str(TXT_MODEL_CONTROL_NUMBER, note());
        }

        // note

        /**
         * @brief Returns the note.
         *
         * @return Note value encoded by the message or model object.
         */
        int note() const { return _controlNumberOrNote; }
        /**
         * @brief Sets the note.
         *
         * @param[in] note Note value supplied to the operation.
         */
        void setNote(int note) {
            _controlNumberOrNote = clamp(note, 0, 127);
        }

        /**
         * @brief Adjusts the note from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editNote(int value, bool shift) {
            setNote(note() + value);
        }

        /**
         * @brief Formats the note into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printNote(StringBuilder &str) const {
            Types::printMidiNote(str, note());
        }

        // noteRange

        /**
         * @brief Returns the note range.
         *
         * @return Configured MIDI note range.
         */
        int noteRange() const { return _noteRange; }
        /**
         * @brief Sets the note range.
         *
         * @param[in] noteRange Width of the accepted MIDI-note range in semitones.
         */
        void setNoteRange(int noteRange) {
            _noteRange = clamp(noteRange, 2, 64);
        }

        /**
         * @brief Adjusts the note range from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editNoteRange(int value, bool shift) {
            setNoteRange(noteRange() + value);
        }

        /**
         * @brief Formats the note range into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printNoteRange(StringBuilder &str) const {
            str(TXT_MODEL_NOTE_RANGE_VALUE, noteRange());
        }

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

        /**
         * @brief Compares this value with another instance for value equality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state is equal; otherwise `false`.
         */
        bool operator==(const MidiSource &other) const;

    private:
        MidiSourceConfig _source; ///< MIDI routing/source configuration.
        Event _event; ///< MIDI event type that this routing source accepts and decodes.
        uint8_t _controlNumberOrNote; ///< MIDI controller number or note number, depending on `_event`, in the 0..127 domain.
        uint8_t _noteRange; ///< Width of the accepted MIDI note range, constrained to 2..64 semitones.
    };

    /**
     * @brief Stores and manipulates route model data.
     */
    class Route {
    public:
        // target

        /**
         * @brief Returns the target.
         *
         * @return Configured routing target.
         */
        Target target() const { return _target; }
        /**
         * @brief Sets the target.
         *
         * @param[in] target Target object/value affected by the operation.
         */
        void setTarget(Target target) {
            target = ModelUtils::clampedEnum(target);
            if (target != _target) {
                _target = target;
                std::tie(_min, _max) = normalizedDefaultRange(target);
            }
        }

        /**
         * @brief Adjusts the target from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editTarget(int value, bool shift) {
            setTarget(ModelUtils::adjustedEnum(target(), value));
        }

        /**
         * @brief Formats the target into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printTarget(StringBuilder &str) const {
            str(targetName(target()));
        }

        // tracks

        /**
         * @brief Returns the tracks.
         *
         * @return Track-selection mask.
         */
        uint8_t tracks() const { return isPerTrackTarget(_target) ? _tracks : 0; }
        /**
         * @brief Sets the tracks.
         *
         * @param[in] tracks Track collection read or modified by the operation.
         */
        void setTracks(uint8_t tracks) {
            if (isPerTrackTarget(_target)) {
                _tracks = tracks;
            }
        }

        /**
         * @brief Toggles track.
         *
         * @param[in] trackIndex Zero-based track index.
         */
        void toggleTrack(int trackIndex) {
            uint8_t trackBit = (1<<trackIndex);
            if (tracks() & trackBit) {
                setTracks(tracks() & ~trackBit);
            } else {
                setTracks(tracks() | trackBit);
            }
        }

        /**
         * @brief Formats the tracks into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printTracks(StringBuilder &str) const {
            if (isPerTrackTarget(_target)) {
                for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
                    str(TXT_MODEL_GENERIC_CHAR, (_tracks & (1<<i)) ? TXT_MODEL_X : TXT_MODEL_NONE_SHORT);
                }
            } else {
                str(TXT_MODEL_NA);
            }
        }

        // min

        /**
         * @brief Returns the min.
         *
         * @return Configured minimum value.
         */
        float min() const { return _min; }
        /**
         * @brief Sets the min.
         *
         * @param[in] min Minimum accepted or represented value.
         */
        void setMin(float min) {
            _min = clamp(min, 0.f, 1.f);
            if (max() < _min) {
                setMax(_min);
            }
        }

        /**
         * @brief Adjusts the min from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editMin(int value, bool shift) {
            setMin(min() + value * targetValueStep(_target, shift));
        }

        /**
         * @brief Formats the min into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printMin(StringBuilder &str) const {
            Routing::printTargetValue(_target, _min, str);
        }

        // max

        /**
         * @brief Returns the max.
         *
         * @return Configured maximum value.
         */
        float max() const { return _max; }
        /**
         * @brief Sets the max.
         *
         * @param[in] max Maximum accepted or represented value.
         */
        void setMax(float max) {
            _max = clamp(max, 0.f, 1.f);
            if (min() > _max) {
                setMin(_max);
            }
        }

        /**
         * @brief Adjusts the max from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editMax(int value, bool shift) {
            setMax(max() + value * targetValueStep(_target, shift));
        }

        /**
         * @brief Formats the max into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printMax(StringBuilder &str) const {
            Routing::printTargetValue(_target, _max, str);
        }

        // source

        /**
         * @brief Returns the source.
         *
         * @return Configured routing source.
         */
        Source source() const { return _source; }
        /**
         * @brief Sets the source.
         *
         * @param[in] source Routing source whose value or configuration is queried or changed.
         */
        void setSource(Source source) {
            _source = ModelUtils::clampedEnum(source);
        }

        /**
         * @brief Adjusts the source from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editSource(int value, bool shift) {
            setSource(ModelUtils::adjustedEnum(source(), value));
        }

        /**
         * @brief Formats the source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printSource(StringBuilder &str) const {
            Routing::printSource(source(), str);
        }

        // cvSource

        /**
         * @brief Returns the cv source.
         *
         * @return Reference to the cv source.
         */
        const CvSource &cvSource() const { return _cvSource; }
              /**
               * @brief Returns the cv source.
               *
               * @return Reference to the cv source.
               */
              CvSource &cvSource()       { return _cvSource; }

        // midiSource

        /**
         * @brief Returns the midi source.
         *
         * @return Reference to the midi source.
         */
        const MidiSource &midiSource() const { return _midiSource; }
              /**
               * @brief Returns the midi source.
               *
               * @return Reference to the midi source.
               */
              MidiSource &midiSource()       { return _midiSource; }

        /**
         * @brief Constructs a Route instance.
         */
        Route();

        /**
         * @brief Clears stored runtime/container state.
         */
        void clear();

        /**
         * @brief Returns the active.
         *
         * @return `true` if active; otherwise `false`.
         */
        bool active() const { return _target != Target::None; }

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
         * @brief Compares this value with another instance for value equality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state is equal; otherwise `false`.
         */
        bool operator==(const Route &other) const;
        /**
         * @brief Compares this value with another instance for value inequality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state differs; otherwise `false`.
         */
        bool operator!=(const Route &other) const {
            return !(*this == other);
        }

    private:
        Target _target; ///< Routing destination selected by this route.
        int8_t _tracks; ///< Bit mask selecting the tracks affected by a per-track routing target.
        float _min; ///< Lower mapped value for the selected routing target.
        float _max; ///< Upper mapped value for the selected routing target.
        Source _source; ///< Primary routing source selected by this route.
        CvSource _cvSource; ///< CV-input source selected when the route uses a CV source.
        MidiSource _midiSource; ///< MIDI source/filter selected when the route uses MIDI input.

        friend class Routing;
    };

    typedef std::array<Route, CONFIG_ROUTE_COUNT> RouteArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // routes

    /**
     * @brief Returns the routes.
     *
     * @return Reference to the routes.
     */
    const RouteArray &routes() const { return _routes; }
          /**
           * @brief Returns the routes.
           *
           * @return Reference to the routes.
           */
          RouteArray &routes()       { return _routes; }

    /**
     * @brief Returns route.
     *
     * @param[in] index Zero-based route index.
     *
     * @return Reference to the route.
     */
    const Route &route(int index) const { return _routes[index]; }
          /**
           * @brief Returns route.
           *
           * @param[in] index Zero-based route index.
           *
           * @return Reference to the route.
           */
          Route &route(int index)       { return _routes[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a Routing instance.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    Routing(Project &project);

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Finds empty route.
     *
     * @return Index of an available routing slot, or the function-defined sentinel when none is available.
     */
    int findEmptyRoute() const;
    /**
     * @brief Finds route.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] trackIndex Zero-based track index.
     *
     * @return Result of findRoute().
     */
    int findRoute(Target target, int trackIndex) const;
    /**
     * @brief Returns check route conflict.
     *
     * @param[in] editedRoute Candidate route after the current edit operation.
     * @param[in] existingRoute Existing route used to detect conflicts with the edited route.
     *
     * @return Result of checkRouteConflict().
     */
    int checkRouteConflict(const Route &editedRoute, const Route &existingRoute) const;

    /**
     * @brief Writes target.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] tracks Track collection read or modified by the operation.
     * @param[in] normalized Normalized value in the range expected by the operation.
     */
    void writeTarget(Target target, uint8_t tracks, float normalized);

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
     * @brief Reports whether dirty.
     *
     * @return `true` if dirty; otherwise `false`.
     */
    bool isDirty() const { return _dirty; }
    /**
     * @brief Clears the model dirty flag after state has been accepted/persisted.
     */
    void clearDirty() { _dirty = false; }

    // global state for keeping active set of routed targets
    /**
     * @brief Reports whether routed.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] trackIndex Zero-based track index.
     *
     * @return `true` if routed; otherwise `false`.
     */
    static bool isRouted(Target target, int trackIndex = -1);
    /**
     * @brief Sets the routed.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] tracks Track collection read or modified by the operation.
     * @param[in] routed `true` to enable/set routed; `false` to disable/clear it.
     */
    static void setRouted(Target target, uint8_t tracks, bool routed);
    /**
     * @brief Formats the routed into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] target Target object/value affected by the operation.
     * @param[in] trackIndex Zero-based track index.
     */
    static void printRouted(StringBuilder &str, Target target, int trackIndex = -1);

private:
    /**
     * @brief Returns normalize target value.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     *
     * @return Result of normalizeTargetValue().
     */
    static float normalizeTargetValue(Target target, float value);
    /**
     * @brief Returns denormalize target value.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] normalized Normalized value in the range expected by the operation.
     *
     * @return Result of denormalizeTargetValue().
     */
    static float denormalizeTargetValue(Target target, float normalized);
    /**
     * @brief Returns normalized default range.
     *
     * @param[in] target Target object/value affected by the operation.
     *
     * @return Result of normalizedDefaultRange().
     */
    static std::pair<float, float> normalizedDefaultRange(Target target);
    /**
     * @brief Returns target value step.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     *
     * @return Result of targetValueStep().
     */
    static float targetValueStep(Target target, bool shift);
    /**
     * @brief Formats the target value into the supplied string builder/output.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] normalized Normalized value in the range expected by the operation.
     * @param[out] str String builder that receives the formatted representation.
     */
    static void printTargetValue(Target target, float normalized, StringBuilder &str);

    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
    RouteArray _routes; ///< Owned fixed-size collection of routes.
    /**
     * @brief Whether persistent model state has changed since it was last accepted/saved.
     */
    bool _dirty; ///< Whether persistent model state has changed since it was last accepted/saved.
};

// Routable parameters store both a base and routed value.
/**
 * @brief Stores and manipulates routable model data.
 */
template<typename T>
/**
 * @brief Stores and manipulates routable model data.
 */
struct Routable {
    /**
     * @brief Stores base and routed values in both named and indexed form.
     */
    union {
        struct {
            T base;
            T routed;
        };
        T values[2];
    };

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] value Logical/state value to store or drive.
     * @param[in] selectRouted Whether to select the routed slot (`true`) instead of the base slot (`false`).
     */
    inline void set(T value, bool selectRouted) { values[selectRouted] = value; }
    /**
     * @brief Returns get.
     *
     * @param[in] selectRouted Whether to select the routed slot (`true`) instead of the base slot (`false`).
     *
     * @return Result of get().
     */
    inline T get(bool selectRouted) const { return values[selectRouted]; }
};
