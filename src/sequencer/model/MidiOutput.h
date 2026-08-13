/**
 * @file MidiOutput.h
 * @brief Declares the MidiOutput component used by the sequencer model.
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

#include <cstdint>
#include <cstring>

class Project;
class NoteSequence;
class CurveSequence;

/**
 * @brief Stores and manipulates midi output model data.
 */
class MidiOutput {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Stores and manipulates output model data.
     */
    class Output {
    public:
        /**
         * @brief Enumerates the supported event values.
         */
        enum class Event : uint8_t {
            None, ///< Represents no selection or an invalid value.
            Note, ///< Selects the note event.
            ControlChange, ///< Selects the control change event.
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
            case Event::None:           return TXT_MODEL_NONE;
            case Event::Note:           return TXT_MODEL_NOTE;
            case Event::ControlChange:  return TXT_MODEL_CC;
            case Event::Last:           break;
            }
            return nullptr;
        }

        /**
         * @brief Enumerates the supported gate source values.
         */
        enum class GateSource : uint8_t {
            FirstTrack, ///< Selects the first track gate source.
            LastTrack = FirstTrack + 7, ///< Selects the last track gate source.
            Last, ///< Sentinel marking the end of the valid enumeration range.
        };

        /**
         * @brief Enumerates the supported note source values.
         */
        enum class NoteSource : uint8_t {
            FirstTrack, ///< Selects the first track note source.
            LastTrack = FirstTrack + 7, ///< Selects the last track note source.
            FirstNote, ///< Selects the first note note source.
            LastNote = FirstNote + 127, ///< Selects the last note note source.
            Last, ///< Sentinel marking the end of the valid enumeration range.
        };

        /**
         * @brief Enumerates the supported velocity source values.
         */
        enum class VelocitySource : uint8_t {
            FirstTrack, ///< Selects the first track velocity source.
            LastTrack = FirstTrack + 7, ///< Selects the last track velocity source.
            FirstVelocity, ///< Selects the first velocity velocity source.
            LastVelocity = FirstVelocity + 127, ///< Selects the last velocity velocity source.
            Last, ///< Sentinel marking the end of the valid enumeration range.
        };

        /**
         * @brief Enumerates the supported control source values.
         */
        enum class ControlSource : uint8_t {
            FirstTrack, ///< Selects the first track control source.
            LastTrack = FirstTrack + 7, ///< Selects the last track control source.
            Last, ///< Sentinel marking the end of the valid enumeration range.
        };

        // target

        /**
         * @brief Returns the target.
         *
         * @return Reference to the target.
         */
        const MidiTargetConfig &target() const { return _target; };
              /**
               * @brief Returns the target.
               *
               * @return Reference to the target.
               */
              MidiTargetConfig &target()       { return _target; };

        // event

        /**
         * @brief Returns the event.
         *
         * @return Current event.
         */
        Event event() const { return _event; }
        /**
         * @brief Sets the event.
         *
         * @param[in] event Event to process.
         * @param[in] force Boolean value controlling force.
         */
        void setEvent(Event event, bool force = false) {
            if (force || event != _event) {
                _event = ModelUtils::clampedEnum(event);
                std::memset(&_data, 0, sizeof(_data));
                switch (event) {
                case Event::None:
                    break;
                case Event::Note:
                    _data.note.velocitySource = VelocitySource(int(VelocitySource::FirstVelocity) + 100);
                    break;
                case Event::ControlChange:
                    break;
                case Event::Last:
                    break;
                }
            }
        }

        /**
         * @brief Adjusts the event from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
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

        // gateSource

        /**
         * @brief Returns the gate source.
         *
         * @return Current gate source.
         */
        GateSource gateSource() const { return _data.note.gateSource; }
        /**
         * @brief Sets the gate source.
         *
         * @param[in] gateSource New gate source value.
         */
        void setGateSource(GateSource gateSource) {
            _data.note.gateSource = ModelUtils::clampedEnum(gateSource);
        }

        /**
         * @brief Adjusts the gate source from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editGateSource(int value, bool shift) {
            setGateSource(ModelUtils::adjustedEnum(gateSource(), value));
        }

        /**
         * @brief Formats the gate source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printGateSource(StringBuilder &str) const {
            printTrackSource(str, gateSource());
        }

        // noteSource

        /**
         * @brief Returns the note source.
         *
         * @return Current note source.
         */
        NoteSource noteSource() const { return _data.note.noteSource; }
        /**
         * @brief Sets the note source.
         *
         * @param[in] noteSource New note source value.
         */
        void setNoteSource(NoteSource noteSource) {
            _data.note.noteSource = ModelUtils::clampedEnum(noteSource);
        }

        /**
         * @brief Adjusts the note source from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editNoteSource(int value, bool shift) {
            setNoteSource(ModelUtils::adjustedEnum(noteSource(), value));
        }

        /**
         * @brief Formats the note source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printNoteSource(StringBuilder &str) const {
            if (!printTrackSource(str, noteSource())) {
                Types::printMidiNote(str, int(noteSource()) - int(NoteSource::FirstNote));
            }
        }

        // velocitySource

        /**
         * @brief Returns the velocity source.
         *
         * @return Current velocity source.
         */
        VelocitySource velocitySource() const { return _data.note.velocitySource; }
        /**
         * @brief Sets the velocity source.
         *
         * @param[in] velocitySource New velocity source value.
         */
        void setVelocitySource(VelocitySource velocitySource) {
            _data.note.velocitySource = ModelUtils::clampedEnum(velocitySource);
        }

        /**
         * @brief Adjusts the velocity source from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editVelocitySource(int value, bool shift) {
            setVelocitySource(ModelUtils::adjustedEnum(velocitySource(), value));
        }

        /**
         * @brief Formats the velocity source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printVelocitySource(StringBuilder &str) const {
            if (!printTrackSource(str, velocitySource())) {
                str(TXT_MODEL_VELOCITY, int(velocitySource()) - int(VelocitySource::FirstVelocity));
            }
        }

        // controlNumber

        /**
         * @brief Returns the control number.
         *
         * @return Current control number.
         */
        int controlNumber() const { return _data.controlChange.controlNumber; }
        /**
         * @brief Sets the control number.
         *
         * @param[in] controlNumber New control number value.
         */
        void setControlNumber(int controlNumber) {
            _data.controlChange.controlNumber = clamp(controlNumber, 0, 127);
        }

        /**
         * @brief Adjusts the control number from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
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
            str(TXT_MODEL_CONTROL_NUMBER, controlNumber());
        }

        // controlSource

        /**
         * @brief Returns the control source.
         *
         * @return Current control source.
         */
        ControlSource controlSource() const { return _data.controlChange.controlSource; }
        /**
         * @brief Sets the control source.
         *
         * @param[in] controlSource New control source value.
         */
        void setControlSource(ControlSource controlSource) {
            _data.controlChange.controlSource = ModelUtils::clampedEnum(controlSource);
        }

        /**
         * @brief Adjusts the control source from a UI edit delta.
         *
         * @param[in] value Replacement value to store or apply.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editControlSource(int value, bool shift) {
            setControlSource(ModelUtils::adjustedEnum(controlSource(), value));
        }

        /**
         * @brief Formats the control source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printControlSource(StringBuilder &str) const {
            if (!printTrackSource(str, controlSource())) {
            }
        }

        /**
         * @brief Reports whether note event.
         *
         * @return `true` if note event; otherwise `false`.
         */
        bool isNoteEvent() const {
            return event() == MidiOutput::Output::Event::Note;
        }

        /**
         * @brief Reports whether control change event.
         *
         * @return `true` if control change event; otherwise `false`.
         */
        bool isControlChangeEvent() const {
            return event() == MidiOutput::Output::Event::ControlChange;
        }

        /**
         * @brief Computes the takes gate from track result.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return `true` if takes gate from track; otherwise `false`.
         */
        bool takesGateFromTrack(int trackIndex) const {
            return isNoteEvent() && int(gateSource()) == trackIndex;
        }

        /**
         * @brief Computes the takes note from track result.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return `true` if takes note from track; otherwise `false`.
         */
        bool takesNoteFromTrack(int trackIndex) const {
            return isNoteEvent() && int(noteSource()) == trackIndex;
        }

        /**
         * @brief Computes the takes velocity from track result.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return `true` if takes velocity from track; otherwise `false`.
         */
        bool takesVelocityFromTrack(int trackIndex) const {
            return isNoteEvent() && int(velocitySource()) == trackIndex;
        }

        /**
         * @brief Computes the takes control from track result.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return `true` if takes control from track; otherwise `false`.
         */
        bool takesControlFromTrack(int trackIndex) const {
            return isControlChangeEvent() && int(controlSource()) == trackIndex;
        }


        /**
         * @brief Clears stored runtime/container state.
         */
        void clear();

        /**
         * @brief Executes write.
         *
         * @param[in,out] writer Writer that receives serialized output data.
         */
        void write(VersionedSerializedWriter &writer) const;
        /**
         * @brief Executes read.
         *
         * @param[in] reader Reader that supplies serialized input data.
         */
        void read(VersionedSerializedReader &reader);

        /**
         * @brief Compares two MIDI output configurations for value equality.
         * @param[in] other Output configuration to compare with this instance.
         * @return `true` when both configurations are equivalent; otherwise `false`.
         */
        bool operator==(const Output &other) const;
        /**
         * @brief Compares two MIDI output configurations for value inequality.
         * @param[in] other Output configuration to compare with this instance.
         * @return `true` when the configurations differ; otherwise `false`.
         */
        bool operator!=(const Output &other) const {
            return !(*this == other);
        }

    private:
        template<typename Enum>
        /**
         * @brief Formats the track source into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         * @param[in] value Replacement value to store or apply.
         *
         * @return `true` if print track source; otherwise `false`.
         */
        static bool printTrackSource(StringBuilder &str, Enum value) {
            if (int(value) >= int(Enum::FirstTrack) && int(value) <= int(Enum::LastTrack)) {
                str(TXT_MODEL_TRACK, int(value) - int(Enum::FirstTrack) + 1);
                return true;
            }
            return false;
        }

        MidiTargetConfig _target; ///< Lifecycle callbacks for the currently attached simulated target.
        Event _event; ///< Event state owned by this component.
        /**
         * @brief Stores the gate source state used by this component.
         */
        union {
            /**
             * @brief Stores and manipulates note model data.
             */
            struct Note {
                GateSource gateSource; ///< Gate source state owned by this component.
                NoteSource noteSource; ///< Note source state owned by this component.
                VelocitySource velocitySource; ///< Velocity source state owned by this component.
                /**
                 * @brief Compares two note-event source configurations.
                 * @param[in] other Note-event source configuration to compare.
                 * @return `true` when all note-event sources are equal; otherwise `false`.
                 */
                bool operator==(const Note &other) const {
                    return gateSource == other.gateSource && noteSource == other.noteSource && velocitySource == other.velocitySource;
                }
            } note; ///< Note state owned by this component.
            /**
             * @brief Stores and manipulates control change model data.
             */
            struct ControlChange {
                uint8_t controlNumber; ///< Persisted control number setting stored with the owning model.
                ControlSource controlSource; ///< Control source state owned by this component.
                /**
                 * @brief Compares two control-change source configurations.
                 * @param[in] other Control-change source configuration to compare.
                 * @return `true` when control number and source are equal; otherwise `false`.
                 */
                bool operator==(const ControlChange &other) const {
                    return controlNumber == other.controlNumber && controlSource == other.controlSource;
                }
            } controlChange; ///< Control change state owned by this component.
        } _data;
    };

    typedef std::array<Output, CONFIG_MIDI_OUTPUT_COUNT> OutputArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // outputs

    /**
     * @brief Returns the outputs.
     *
     * @return Reference to the outputs.
     */
    const OutputArray &outputs() const { return _outputs; }
          /**
           * @brief Returns the outputs.
           *
           * @return Reference to the outputs.
           */
          OutputArray &outputs()       { return _outputs; }

    /**
     * @brief Computes the output result.
     *
     * @param[in] index Zero-based index of the addressed entry.
     *
     * @return Reference to the output.
     */
    const Output &output(int index) const { return _outputs[index]; }
          /**
           * @brief Computes the output result.
           *
           * @param[in] index Zero-based index of the addressed entry.
           *
           * @return Reference to the output.
           */
          Output &output(int index)       { return _outputs[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a MidiOutput instance.
     */
    MidiOutput();

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Executes write.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Executes read.
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

private:
    OutputArray _outputs; ///< Outputs state owned by this component.
    bool _dirty; ///< Whether persistent model state has changed since it was last accepted/saved.
};
