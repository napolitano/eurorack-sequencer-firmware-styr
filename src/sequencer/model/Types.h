/**
 * @file Types.h
 * @brief Declares the Types component used by the sequencer model.
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
#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <array>

#include <cstdint>

/**
 * @brief Stores and manipulates types model data.
 */
class Types {
public:
    // MonitorMode

    /**
     * @brief Enumerates the supported monitor mode values.
     */
    enum class MonitorMode : uint8_t {
        Always, ///< Selects the always monitor mode.
        Stopped, ///< Selects the stopped monitor mode.
        Off, ///< Selects the off monitor mode.
        Last, ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for monitor mode.
     *
     * @param[in] monitorMode Monitoring policy used while recording or auditioning the track.
     *
     * @return Pointer to the monitor mode name; `nullptr` when no value is available.
     */
    static const char *monitorModeName(MonitorMode monitorMode) {
        switch (monitorMode) {
        case MonitorMode::Always:   return TXT_MODEL_ALWAYS;
        case MonitorMode::Stopped:  return TXT_LIST_LABEL_STOPPED;
        case MonitorMode::Off:      return TXT_MODEL_PRINT_OCTAVE_OFF;
        case MonitorMode::Last:     break;
        }
        return nullptr;
    }

    // RecordMode

    /**
     * @brief Enumerates the supported record mode values.
     */
    enum class RecordMode : uint8_t {
        Overdub, ///< Selects the overdub record mode.
        Overwrite, ///< Selects the overwrite record mode.
        StepRecord, ///< Selects the step record record mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for record mode.
     *
     * @param[in] recordMode Recording mode used to place incoming events.
     *
     * @return Pointer to the record mode name; `nullptr` when no value is available.
     */
    static const char *recordModeName(RecordMode recordMode) {
        switch (recordMode) {
        case RecordMode::Overdub:   return TXT_LIST_LABEL_OVERDUB;
        case RecordMode::Overwrite: return TXT_LIST_LABEL_OVERWRITE;
        case RecordMode::StepRecord:return TXT_LIST_LABEL_STEP_RECORD;
        case RecordMode::Last:      break;
        }
        return nullptr;
    }

    // MidiInputMode

    /**
     * @brief Enumerates the supported midi input mode values.
     */
    enum class MidiInputMode : uint8_t {
        Off, ///< Selects the off midi input mode.
        All, ///< Selects the all midi input mode.
        Source, ///< Selects the source midi input mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    // CvGateInput

    /**
     * @brief Enumerates the supported cv gate input values.
     */
    enum class CvGateInput : uint8_t {
        Off, ///< Selects the off cv gate input.
        Cv1Cv2, ///< Selects the cv1 cv2 cv gate input.
        Cv3Cv4, ///< Selects the cv3 cv4 cv gate input.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for cv gate input.
     *
     * @param[in] cvGateInput Configured CV/gate input mapping used by the track.
     *
     * @return Pointer to the cv gate input name; `nullptr` when no value is available.
     */
    static const char *cvGateInputName(CvGateInput cvGateInput) {
        switch (cvGateInput) {
        case CvGateInput::Off:      return TXT_MODEL_PRINT_OCTAVE_OFF;
        case CvGateInput::Cv1Cv2:   return TXT_LIST_LABEL_CV1_CV2;
        case CvGateInput::Cv3Cv4:   return TXT_LIST_LABEL_CV3_CV4;
        case CvGateInput::Last:     break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported curve cv input values.
     */
    enum class CurveCvInput : uint8_t {
        Off, ///< Selects the off curve cv input.
        Cv1, ///< Selects the cv1 curve cv input.
        Cv2, ///< Selects the cv2 curve cv input.
        Cv3, ///< Selects the cv3 curve cv input.
        Cv4, ///< Selects the cv4 curve cv input.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns curve cv input.
     *
     * @param[in] curveCvInput CV input mapping used while recording curve data.
     *
     * @return Pointer to the curve cv input; `nullptr` when no value is available.
     */
    static const char *curveCvInput(CurveCvInput curveCvInput) {
        switch (curveCvInput) {
        case CurveCvInput::Off:     return TXT_MODEL_PRINT_OCTAVE_OFF;
        case CurveCvInput::Cv1:     return TXT_LIST_LABEL_CV1;
        case CurveCvInput::Cv2:     return TXT_LIST_LABEL_CV2;
        case CurveCvInput::Cv3:     return TXT_LIST_LABEL_CV3;
        case CurveCvInput::Cv4:     return TXT_LIST_LABEL_CV4;
        case CurveCvInput::Last:    break;
        }
        return nullptr;
    }

    // PlayMode

    /**
     * @brief Enumerates the supported play mode values.
     */
    enum class PlayMode : uint8_t {
        Aligned, ///< Selects the aligned play mode.
        Free, ///< Selects the free play mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for play mode.
     *
     * @param[in] playMode Playback mode that determines transport behavior.
     *
     * @return Pointer to the play mode name; `nullptr` when no value is available.
     */
    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Aligned: return TXT_LIST_LABEL_ALIGNED;
        case PlayMode::Free:    return TXT_LIST_LABEL_FREE;
        case PlayMode::Last:    break;
        }
        return nullptr;
    }

    // RunMode

    /**
     * @brief Enumerates the supported run mode values.
     */
    enum class RunMode : uint8_t {
        Forward, ///< Selects the forward run mode.
        Backward, ///< Selects the backward run mode.
        Pendulum, ///< Selects the pendulum run mode.
        PingPong, ///< Selects the ping pong run mode.
        Random, ///< Selects the random run mode.
        RandomWalk, ///< Selects the random walk run mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for run mode.
     *
     * @param[in] runMode Sequence run mode controlling traversal order.
     *
     * @return Pointer to the run mode name; `nullptr` when no value is available.
     */
    static const char *runModeName(RunMode runMode) {
        switch (runMode) {
        case RunMode::Forward:      return TXT_LIST_LABEL_FORWARD;
        case RunMode::Backward:     return TXT_LIST_LABEL_BACKWARD;
        case RunMode::Pendulum:     return TXT_LIST_LABEL_PENDULUM;
        case RunMode::PingPong:     return TXT_LIST_LABEL_PINGPONG;
        case RunMode::Random:       return TXT_LIST_LABEL_RANDOM;
        case RunMode::RandomWalk:   return TXT_LIST_LABEL_RANDOM_WALK;
        case RunMode::Last:         break;
        }
        return nullptr;
    }

    // Condition

    /**
     * @brief Enumerates the supported condition values.
     */
    enum class Condition : uint8_t {
        Off, ///< Selects the off condition.
        Fill, ///< Selects the fill condition.
        NotFill, ///< Selects the not fill condition.
        Pre, ///< Selects the pre condition.
        NotPre, ///< Selects the not pre condition.
        First, ///< Selects the first condition.
        NotFirst, ///< Selects the not first condition.
        Loop, ///< Selects the loop condition.
        Loop2 = Loop, ///< Selects the loop2 condition.
        Loop3 = Loop2 + 2, ///< Selects the loop3 condition.
        Loop4 = Loop3 + 3, ///< Selects the loop4 condition.
        Loop5 = Loop4 + 4, ///< Selects the loop5 condition.
        Loop6 = Loop5 + 5, ///< Selects the loop6 condition.
        Loop7 = Loop6 + 6, ///< Selects the loop7 condition.
        Loop8 = Loop7 + 7, ///< Selects the loop8 condition.
        NotLoop = Loop8 + 8, ///< Selects the not loop condition.
        NotLoop2 = NotLoop, ///< Selects the not loop2 condition.
        NotLoop3 = NotLoop2 + 2, ///< Selects the not loop3 condition.
        NotLoop4 = NotLoop3 + 3, ///< Selects the not loop4 condition.
        NotLoop5 = NotLoop4 + 4, ///< Selects the not loop5 condition.
        NotLoop6 = NotLoop5 + 5, ///< Selects the not loop6 condition.
        NotLoop7 = NotLoop6 + 6, ///< Selects the not loop7 condition.
        NotLoop8 = NotLoop7 + 7, ///< Selects the not loop8 condition.
        Last = NotLoop8 + 8 ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Stores and manipulates condition info model data.
     */
    struct ConditionInfo {
        /**
         * @brief Pointer to name; `nullptr` denotes that no object/resource is assigned.
         */
        const char *name; ///< Pointer to name; `nullptr` denotes that no object/resource is assigned.
        /**
         * @brief Pointer to short1; `nullptr` denotes that no object/resource is assigned.
         */
        const char *short1; ///< Pointer to short1; `nullptr` denotes that no object/resource is assigned.
        /**
         * @brief Pointer to short2; `nullptr` denotes that no object/resource is assigned.
         */
        const char *short2; ///< Pointer to short2; `nullptr` denotes that no object/resource is assigned.
    };

    /**
     * @brief Stores and manipulates condition loop model data.
     */
    struct ConditionLoop {
        /**
         * @brief Offset applied to  in this type's timing/coordinate domain.
         */
        uint8_t offset; ///< Offset applied to  in this type's timing/coordinate domain.
        uint8_t base; ///< Base state used by `Types` to preserve its current configuration or runtime progress.
        uint8_t invert; ///< Invert state used by `Types` to preserve its current configuration or runtime progress.
    };

    /**
     * @brief Enumerates the supported condition format values.
     */
    enum class ConditionFormat : uint8_t {
        Long, ///< Selects the long condition format.
        Short1, ///< Selects the short1 condition format.
        Short2 ///< Selects the short2 condition format.
    };

    /**
     * @brief Returns condition loop.
     *
     * @param[in] condition Step/play condition evaluated or stored by the operation.
     *
     * @return Result of conditionLoop().
     */
    static ConditionLoop conditionLoop(Condition condition) {
        static const uint8_t offset[] = { 0, 1,   0, 1, 2,   0, 1, 2, 3,   0, 1, 2, 3, 4,   0, 1, 2, 3, 4, 5,   0, 1, 2, 3, 4, 5, 6,   0, 1, 2, 3, 4, 5, 6, 7 };
        static const uint8_t base[]   = { 2, 2,   3, 3, 3,   4, 4, 4, 4,   5, 5, 5, 5, 5,   6, 6, 6, 6, 6, 6,   7, 7, 7, 7, 7, 7, 7,   8, 8, 8, 8, 8, 8, 8, 8 };
        int index = int(condition);
        if (index >= int(Condition::Loop) && index < int(Condition::NotLoop)) {
            index -= int(Condition::Loop);
            return { offset[index], base[index], 0 };
        } else if (index >= int(Condition::NotLoop) && index < int(Condition::Last)) {
            index -= int(Condition::NotLoop);
            return { offset[index], base[index], 1 };
        } else {
            return { 0, 0, 0 };
        }
    }

    /**
     * @brief Formats the condition into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] condition Step/play condition evaluated or stored by the operation.
     * @param[in] format Formatting callback or format selector used to render the value.
     */
    static void printCondition(StringBuilder &str, Condition condition, ConditionFormat format = ConditionFormat::Long) {
        int index = int(condition);
        if (index >= 0 && index < int(Condition::Loop)) {
            const auto &info = conditionInfos[index];
            switch (format) {
            case ConditionFormat::Long: str(info.name); break;
            case ConditionFormat::Short1: str(info.short1); break;
            case ConditionFormat::Short2: str(info.short2); break;
            }
        } else if (index >= int(Condition::Loop) && index < int(Condition::Last)) {
            auto loop = conditionLoop(condition);
            switch (format) {
            case ConditionFormat::Long: str("%s%d:%d", loop.invert ? "!" : "", loop.offset + 1, loop.base); break;
            case ConditionFormat::Short1: str("%s%d", loop.invert ? "!" : "", loop.offset + 1); break;
            case ConditionFormat::Short2: str("%d", loop.base); break;
            }
        }
    }

    // VoltageRange

    /**
     * @brief Enumerates the supported voltage range values.
     */
    enum class VoltageRange : uint8_t {
        Unipolar1V, ///< Selects the unipolar1 v voltage range.
        Unipolar2V, ///< Selects the unipolar2 v voltage range.
        Unipolar3V, ///< Selects the unipolar3 v voltage range.
        Unipolar4V, ///< Selects the unipolar4 v voltage range.
        Unipolar5V, ///< Selects the unipolar5 v voltage range.
        Bipolar1V, ///< Selects the bipolar1 v voltage range.
        Bipolar2V, ///< Selects the bipolar2 v voltage range.
        Bipolar3V, ///< Selects the bipolar3 v voltage range.
        Bipolar4V, ///< Selects the bipolar4 v voltage range.
        Bipolar5V, ///< Selects the bipolar5 v voltage range.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for voltage range.
     *
     * @param[in] voltageRange Voltage range used for CV conversion.
     *
     * @return Pointer to the voltage range name; `nullptr` when no value is available.
     */
    static const char *voltageRangeName(VoltageRange voltageRange) {
        switch (voltageRange) {
        case VoltageRange::Unipolar1V:  return TXT_LIST_LABEL_1V_UNIPOLAR;
        case VoltageRange::Unipolar2V:  return TXT_LIST_LABEL_2V_UNIPOLAR;
        case VoltageRange::Unipolar3V:  return TXT_LIST_LABEL_3V_UNIPOLAR;
        case VoltageRange::Unipolar4V:  return TXT_LIST_LABEL_4V_UNIPOLAR;
        case VoltageRange::Unipolar5V:  return TXT_LIST_LABEL_5V_UNIPOLAR;
        case VoltageRange::Bipolar1V:   return TXT_LIST_LABEL_1V_BIPOLAR;
        case VoltageRange::Bipolar2V:   return TXT_LIST_LABEL_2V_BIPOLAR;
        case VoltageRange::Bipolar3V:   return TXT_LIST_LABEL_3V_BIPOLAR;
        case VoltageRange::Bipolar4V:   return TXT_LIST_LABEL_4V_BIPOLAR;
        case VoltageRange::Bipolar5V:   return TXT_LIST_LABEL_5V_BIPOLAR;
        case VoltageRange::Last:        break;
        }
        return nullptr;
    }

    /**
     * @brief Stores and manipulates voltage range info model data.
     */
    struct VoltageRangeInfo {
        float lo; ///< Lower endpoint of the floating-point range.
        float hi; ///< Upper endpoint of the floating-point range.

        /**
         * @brief Normalizes the value to this type's canonical range.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         *
         * @return Normalized value.
         */
        float normalize(float value) const {
            return clamp((value - lo) / (hi - lo), 0.f, 1.f);
        }

        /**
         * @brief Returns denormalize.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         *
         * @return Value converted back from normalized representation.
         */
        float denormalize(float value) const {
            return clamp(value, 0.f, 1.f) * (hi - lo) + lo;
        }
    };

    /**
     * @brief Returns voltage range info.
     *
     * @param[in] voltageRange Voltage range used for CV conversion.
     *
     * @return Reference to the voltage range info.
     */
    static const VoltageRangeInfo &voltageRangeInfo(VoltageRange voltageRange) {
        return voltageRangeInfos[int(voltageRange)];
    }

    // MidiPort

    /**
     * @brief Enumerates the supported midi port values.
     */
    enum class MidiPort : uint8_t {
        Midi, ///< Selects the midi midi port.
        UsbMidi, ///< Selects the usb midi midi port.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for midi port.
     *
     * @param[in] midiPort MIDI port from which the message is received or to which it is sent.
     *
     * @return Pointer to the midi port name; `nullptr` when no value is available.
     */
    static const char *midiPortName(MidiPort midiPort) {
        switch (midiPort) {
        case MidiPort::Midi:    return TXT_LIST_LABEL_MIDI;
        case MidiPort::UsbMidi: return TXT_LIST_LABEL_USB;
        case MidiPort::Last:    break;
        }
        return nullptr;
    }

    // Misc types

    /**
     * @brief Stores and manipulates layer range model data.
     */
    struct LayerRange {
        int min; ///< Inclusive lower layer index.
        int max; ///< Inclusive upper layer index.
    };

    // Utilities
    // TODO maybe move these

    /**
     * @brief Formats the midi channel into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] midiChannel MIDI channel number in the model/transport domain.
     */
    static void printMidiChannel(StringBuilder &str, int midiChannel) {
        if (midiChannel == -1) {
            str(TXT_LIST_LABEL_OMNI);
        } else {
            str(TXT_MODEL_GENERIC_VALUE, midiChannel + 1);
        }
    }

    /**
     * @brief Formats the note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] note Note value supplied to the operation.
     */
    static void printNote(StringBuilder &str, int note) {
        static const char *names[] = TXT_NOTE_NAMES;
        str(names[note]);
    }

    /**
     * @brief Formats the midi note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     * @param[in] midiNote MIDI note number in the standard 0..127 domain.
     */
    static void printMidiNote(StringBuilder &str, int midiNote) {
        printNote(str, midiNote % 12);
        int octave = midiNote / 12 - 1;
        str(TXT_MODEL_GENERIC_VALUE, octave);
    }

private:
    /**
     * @brief Condition infos constant used by this component.
     */
    static const ConditionInfo conditionInfos[]; ///< Lookup table containing display/behavior metadata for each note condition.
    /**
     * @brief Voltage range infos constant used by this component.
     */
    static const VoltageRangeInfo voltageRangeInfos[]; ///< Lookup table describing each supported CV voltage range.

}; // namespace Types
