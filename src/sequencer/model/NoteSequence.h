/**
 * @file NoteSequence.h
 * @brief Declares the NoteSequence component used by the sequencer model.
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
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Scale.h"
#include "Routing.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <initializer_list>

/**
 * @brief Stores and manipulates note sequence model data.
 */
class NoteSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<3> GateProbability;
    typedef SignedValue<4> GateOffset;
    typedef UnsignedValue<2> Retrigger;
    typedef UnsignedValue<3> RetriggerProbability;
    typedef UnsignedValue<3> Length;
    typedef SignedValue<4> LengthVariationRange;
    typedef UnsignedValue<3> LengthVariationProbability;
    typedef SignedValue<7> Note;
    typedef SignedValue<7> NoteVariationRange;
    typedef UnsignedValue<3> NoteVariationProbability;
    typedef UnsignedValue<7> Condition;

    static_assert(int(Types::Condition::Last) <= Condition::Max + 1, "Condition enum does not fit");

    /**
     * @brief Enumerates the supported layer values.
     */
    enum class Layer {
        Gate, ///< Selects the gate layer.
        GateProbability, ///< Selects the gate probability layer.
        GateOffset, ///< Selects the gate offset layer.
        Slide, ///< Selects the slide layer.
        Retrigger, ///< Selects the retrigger layer.
        RetriggerProbability, ///< Selects the retrigger probability layer.
        Length, ///< Selects the length layer.
        LengthVariationRange, ///< Selects the length variation range layer.
        LengthVariationProbability, ///< Selects the length variation probability layer.
        Note, ///< Selects the note layer.
        NoteVariationRange, ///< Selects the note variation range layer.
        NoteVariationProbability, ///< Selects the note variation probability layer.
        Condition, ///< Selects the condition layer.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for layer.
     *
     * @param[in] layer Layer index or layer identifier.
     *
     * @return Pointer to the layer name; `nullptr` when no value is available.
     */
    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Gate:                       return TXT_MODEL_GATE;
        case Layer::GateProbability:            return TXT_MODEL_GATE_PROBABILITY;
        case Layer::GateOffset:                 return TXT_MODEL_NOTE_GATE_OFFSET;
        case Layer::Slide:                      return TXT_MODEL_NOTE_SLIDE;
        case Layer::Retrigger:                  return TXT_MODEL_NOTE_RETRIGGER;
        case Layer::RetriggerProbability:       return TXT_MODEL_NOTE_RETRIGGER_PROBABILITY;
        case Layer::Length:                     return TXT_MODEL_NOTE_LENGTH;
        case Layer::LengthVariationRange:       return TXT_MODEL_NOTE_LENGTH_RANGE;
        case Layer::LengthVariationProbability: return TXT_MODEL_NOTE_LENGTH_PROBABILITY;
        case Layer::Note:                       return TXT_MODEL_NOTE;
        case Layer::NoteVariationRange:         return TXT_MODEL_NOTE_RANGE;
        case Layer::NoteVariationProbability:   return TXT_MODEL_NOTE_PROBABILITY;
        case Layer::Condition:                  return TXT_MODEL_NOTE_CONDITION;
        case Layer::Last:                       break;
        }
        return nullptr;
    }

    /**
     * @brief Returns layer range.
     *
     * @param[in] layer Layer index or layer identifier.
     *
     * @return Result of layerRange().
     */
    static Types::LayerRange layerRange(Layer layer);
    /**
     * @brief Returns layer default value.
     *
     * @param[in] layer Layer index or layer identifier.
     *
     * @return Result of layerDefaultValue().
     */
    static int layerDefaultValue(Layer layer);
    /**
     * @brief Remaps note values from one scale definition to another.
     *
     * @param[in] oldScale Scale definition currently encoded by the note data.
     * @param[in] newScale Replacement scale used when remapping note data.
     */
    void remapScale(const Scale &oldScale, const Scale &newScale);
    /**
     * @brief Remaps note values from one scale definition to another.
     *
     * @param[in] oldScaleIndex Zero-based old scale index.
     * @param[in] newScaleIndex Zero-based new scale index.
     * @param[in] defaultScale Project/default scale used for note conversion.
     */
    void remapScale(int oldScaleIndex, int newScaleIndex, int defaultScale);
    /**
     * @brief Stores and manipulates step model data.
     */
    class Step {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // gate

        /**
         * @brief Returns the gate.
         *
         * @return `true` if gate; otherwise `false`.
         */
        bool gate() const { return _data0.gate ? true : false; }
        /**
         * @brief Sets the gate.
         *
         * @param[in] gate `true` to enable/set gate; `false` to disable/clear it.
         */
        void setGate(bool gate) { _data0.gate = gate; }
        /**
         * @brief Toggles gate.
         */
        void toggleGate() { setGate(!gate()); }

        // gateProbability

        /**
         * @brief Returns the gate probability.
         *
         * @return Gate probability in the model-defined 0..100 range.
         */
        int gateProbability() const { return _data0.gateProbability; }
        /**
         * @brief Sets the gate probability.
         *
         * @param[in] gateProbability New gate probability in the 0..100 range to store or apply.
         */
        void setGateProbability(int gateProbability) {
            _data0.gateProbability = GateProbability::clamp(gateProbability);
        }

        // gateOffset

        /**
         * @brief Returns the gate offset.
         *
         * @return Configured note gate timing offset.
         */
        int gateOffset() const { return GateOffset::Min + _data1.gateOffset; }
        /**
         * @brief Sets the gate offset.
         *
         * @param[in] gateOffset Gate offset consumed by `setGateOffset()`.
         */
        void setGateOffset(int gateOffset) {
            _data1.gateOffset = GateOffset::clamp(gateOffset) - GateOffset::Min;
        }

        // slide

        /**
         * @brief Returns the slide.
         *
         * @return `true` if slide; otherwise `false`.
         */
        bool slide() const { return _data0.slide ? true : false; }
        /**
         * @brief Sets the slide.
         *
         * @param[in] slide `true` to enable/set slide; `false` to disable/clear it.
         */
        void setSlide(bool slide) {
            _data0.slide = slide;
        }
        /**
         * @brief Toggles slide.
         */
        void toggleSlide() {
            setSlide(!slide());
        }

        // retrigger

        /**
         * @brief Returns the retrigger.
         *
         * @return Configured retrigger count.
         */
        int retrigger() const { return _data1.retrigger; }
        /**
         * @brief Sets the retrigger.
         *
         * @param[in] retrigger Retrigger consumed by `setRetrigger()`.
         */
        void setRetrigger(int retrigger) {
            _data1.retrigger = Retrigger::clamp(retrigger);
        }

        // retriggerProbability

        /**
         * @brief Returns the retrigger probability.
         *
         * @return Probability of applying retriggers.
         */
        int retriggerProbability() const { return _data1.retriggerProbability; }
        /**
         * @brief Sets the retrigger probability.
         *
         * @param[in] retriggerProbability New retrigger probability in the 0..100 range to store or apply.
         */
        void setRetriggerProbability(int retriggerProbability) {
            _data1.retriggerProbability = RetriggerProbability::clamp(retriggerProbability);
        }

        // length

        /**
         * @brief Returns the length.
         *
         * @return Number of length represented by this object.
         */
        int length() const { return _data0.length; }
        /**
         * @brief Sets the length.
         *
         * @param[in] length Number of bytes or elements covered by the operation.
         */
        void setLength(int length) {
            _data0.length = Length::clamp(length);
        }

        // lengthVariationRange

        /**
         * @brief Returns the length variation range.
         *
         * @return Configured gate-length variation range.
         */
        int lengthVariationRange() const { return LengthVariationRange::Min + _data0.lengthVariationRange; }
        /**
         * @brief Sets the length variation range.
         *
         * @param[in] lengthVariationRange Length variation range consumed by `setLengthVariationRange()`.
         */
        void setLengthVariationRange(int lengthVariationRange) {
            _data0.lengthVariationRange = LengthVariationRange::clamp(lengthVariationRange) - LengthVariationRange::Min;
        }

        // lengthVariationProbability

        /**
         * @brief Returns the length variation probability.
         *
         * @return Probability of applying gate-length variation.
         */
        int lengthVariationProbability() const { return _data0.lengthVariationProbability; }
        /**
         * @brief Sets the length variation probability.
         *
         * @param[in] lengthVariationProbability New length-variation probability in the 0..100 range to store or apply.
         */
        void setLengthVariationProbability(int lengthVariationProbability) {
            _data0.lengthVariationProbability = LengthVariationProbability::clamp(lengthVariationProbability);
        }

        // note

        /**
         * @brief Returns the note.
         *
         * @return Note value encoded by the message or model object.
         */
        int note() const { return Note::Min + _data0.note; }
        /**
         * @brief Sets the note.
         *
         * @param[in] note Note value supplied to the operation.
         */
        void setNote(int note) {
            _data0.note = Note::clamp(note) - Note::Min;
        }

        // noteVariationRange

        /**
         * @brief Returns the note variation range.
         *
         * @return Configured note variation range.
         */
        int noteVariationRange() const { return NoteVariationRange::Min + _data0.noteVariationRange; }
        /**
         * @brief Sets the note variation range.
         *
         * @param[in] noteVariationRange Note variation range consumed by `setNoteVariationRange()`.
         */
        void setNoteVariationRange(int noteVariationRange) {
            _data0.noteVariationRange = NoteVariationRange::clamp(noteVariationRange) - NoteVariationRange::Min;
        }

        // noteVariationProbability

        /**
         * @brief Returns the note variation probability.
         *
         * @return Probability of applying note variation.
         */
        int noteVariationProbability() const { return _data0.noteVariationProbability; }
        /**
         * @brief Sets the note variation probability.
         *
         * @param[in] noteVariationProbability New note-variation probability in the 0..100 range to store or apply.
         */
        void setNoteVariationProbability(int noteVariationProbability) {
            _data0.noteVariationProbability = NoteVariationProbability::clamp(noteVariationProbability);
        }

        // condition

        /**
         * @brief Returns the condition.
         *
         * @return Configured step condition.
         */
        Types::Condition condition() const { return Types::Condition(int(_data1.condition)); }
        /**
         * @brief Sets the condition.
         *
         * @param[in] condition Step/play condition evaluated or stored by the operation.
         */
        void setCondition(Types::Condition condition) {
            _data1.condition = int(ModelUtils::clampedEnum(condition));
        }

        /**
         * @brief Returns layer value.
         *
         * @param[in] layer Layer index or layer identifier.
         *
         * @return Result of layerValue().
         */
        int layerValue(Layer layer) const;
        /**
         * @brief Sets the layer value.
         *
         * @param[in] layer Layer index or layer identifier.
         * @param[in] value New layer value to store or apply.
         */
        void setLayerValue(Layer layer, int value);

        //----------------------------------------
        // Methods
        //----------------------------------------

        /**
         * @brief Constructs a Step instance.
         */
        Step() { clear(); }

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
        bool operator==(const Step &other) const {
            return _data0.raw == other._data0.raw && _data1.raw == other._data1.raw;
        }

        /**
         * @brief Compares this value with another instance for value inequality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state differs; otherwise `false`.
         */
        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        /**
         * @brief Raw encoded bytes that make up the value/message.
         */
        union {
            uint32_t raw;
            BitField<uint32_t, 0, 1> gate;
            BitField<uint32_t, 1, 1> slide;
            BitField<uint32_t, 2, GateProbability::Bits> gateProbability;
            BitField<uint32_t, 5, Length::Bits> length;
            BitField<uint32_t, 8, LengthVariationRange::Bits> lengthVariationRange;
            BitField<uint32_t, 12, LengthVariationProbability::Bits> lengthVariationProbability;
            BitField<uint32_t, 15, Note::Bits> note;
            BitField<uint32_t, 22, NoteVariationRange::Bits> noteVariationRange;
            BitField<uint32_t, 29, NoteVariationProbability::Bits> noteVariationProbability;
        } _data0;
        /**
         * @brief Raw encoded bytes that make up the value/message.
         */
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Retrigger::Bits> retrigger;
            BitField<uint32_t, 2, RetriggerProbability::Bits> retriggerProbability;
            BitField<uint32_t, 5, GateOffset::Bits> gateOffset;
            BitField<uint32_t, 9, Condition::Bits> condition;
            // 16 bits left
        } _data1;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

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

    // scale

    /**
     * @brief Returns the scale.
     *
     * @return Configured scale selection.
     */
    int scale() const { return _scale.get(isRouted(Routing::Target::Scale)); }
    /**
     * @brief Sets the scale.
     *
     * @param[in] scale Scale definition used for quantization or display.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setScale(int scale, bool routed = false) {
        _scale.set(clamp(scale, -1, Scale::Count - 1), routed);
    }

    /**
     * @brief Returns the indexed scale.
     *
     * @return Zero-based indexed scale.
     */
    int indexedScale() const { return scale() + 1; }
    /**
     * @brief Sets the indexed scale.
     *
     * @param[in] index Zero-based indexed scale index.
     */
    void setIndexedScale(int index) {
        setScale(index - 1);
    }

    /**
     * @brief Adjusts the scale from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editScale(int value, bool shift) {
        if (!isRouted(Routing::Target::Scale)) {
            setScale(scale() + value);
        }
    }

    /**
     * @brief Formats the scale into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printScale(StringBuilder &str) const {
        printRouted(str, Routing::Target::Scale);
        str(scale() < 0 ? TXT_MODEL_DEFAULT : Scale::name(scale()));
    }

    /**
     * @brief Selects ed scale.
     *
     * @param[in] defaultScale Project/default scale used for note conversion.
     *
     * @return Reference to the selected scale.
     */
    const Scale &selectedScale(int defaultScale) const {
        return Scale::get(scale() < 0 ? defaultScale : scale());
    }

    // rootNote

    /**
     * @brief Returns the root note.
     *
     * @return Configured root-note selection.
     */
    int rootNote() const { return _rootNote.get(isRouted(Routing::Target::RootNote)); }
    /**
     * @brief Sets the root note.
     *
     * @param[in] rootNote New chromatic root-note index to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setRootNote(int rootNote, bool routed = false) {
        _rootNote.set(clamp(rootNote, -1, 11), routed);
    }

    /**
     * @brief Returns the indexed root note.
     *
     * @return Zero-based indexed root note.
     */
    int indexedRootNote() const { return rootNote() + 1; }
    /**
     * @brief Sets the indexed root note.
     *
     * @param[in] index Zero-based indexed root note index.
     */
    void setIndexedRootNote(int index) {
        setRootNote(index - 1);
    }

    /**
     * @brief Adjusts the root note from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRootNote(int value, bool shift) {
        if (!isRouted(Routing::Target::RootNote)) {
            setRootNote(rootNote() + value);
        }
    }

    /**
     * @brief Formats the root note into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRootNote(StringBuilder &str) const {
        printRouted(str, Routing::Target::RootNote);
        if (rootNote() < 0) {
            str(TXT_MODEL_DEFAULT);
        } else {
            Types::printNote(str, rootNote());
        }
    }

    /**
     * @brief Selects ed root note.
     *
     * @param[in] defaultRootNote Fallback root note used when the source data does not override it.
     *
     * @return Result of selectedRootNote().
     */
    int selectedRootNote(int defaultRootNote) const {
        return rootNote() < 0 ? defaultRootNote : rootNote();
    }

    // divisor

    /**
     * @brief Returns the divisor.
     *
     * @return Configured clock/sequence divisor.
     */
    int divisor() const { return _divisor.get(isRouted(Routing::Target::Divisor)); }
    /**
     * @brief Sets the divisor.
     *
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setDivisor(int divisor, bool routed = false) {
        _divisor.set(ModelUtils::clampDivisor(divisor), routed);
    }

    /**
     * @brief Returns the indexed divisor.
     *
     * @return Zero-based indexed divisor.
     */
    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    /**
     * @brief Sets the indexed divisor.
     *
     * @param[in] index Zero-based indexed divisor index.
     */
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
    }

    /**
     * @brief Adjusts the divisor from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editDivisor(int value, bool shift) {
        if (!isRouted(Routing::Target::Divisor)) {
            setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
        }
    }

    /**
     * @brief Formats the divisor into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printDivisor(StringBuilder &str) const {
        printRouted(str, Routing::Target::Divisor);
        ModelUtils::printDivisor(str, divisor());
    }

    // resetMeasure

    /**
     * @brief Resets measure.
     *
     * @return Measure interval used for automatic sequence reset; zero disables the reset.
     */
    int resetMeasure() const { return _resetMeasure; }
    /**
     * @brief Sets the reset measure.
     *
     * @param[in] resetMeasure Reset measure consumed by `setResetMeasure()`.
     */
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    /**
     * @brief Adjusts the reset measure from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editResetMeasure(int value, bool shift) {
        setResetMeasure(ModelUtils::adjustedByPowerOfTwo(resetMeasure(), value, shift));
    }

    /**
     * @brief Formats the reset measure into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printResetMeasure(StringBuilder &str) const {
        if (resetMeasure() == 0) {
            str(TXT_MODEL_PRINT_OFF);
        } else {
            str(TXT_MODEL_PRINT_BAR_COUNT, resetMeasure(), resetMeasure() > 1 ? TXT_MODEL_PRINT_BAR_PLURAL : TXT_MODEL_PRINT_BAR);
        }
    }

    // runMode

    /**
     * @brief Returns the run mode.
     *
     * @return Configured run mode.
     */
    Types::RunMode runMode() const { return _runMode.get(isRouted(Routing::Target::RunMode)); }
    /**
     * @brief Sets the run mode.
     *
     * @param[in] runMode Sequence run mode controlling traversal order.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setRunMode(Types::RunMode runMode, bool routed = false) {
        _runMode.set(ModelUtils::clampedEnum(runMode), routed);
    }

    /**
     * @brief Adjusts the run mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editRunMode(int value, bool shift) {
        if (!isRouted(Routing::Target::RunMode)) {
            setRunMode(ModelUtils::adjustedEnum(runMode(), value));
        }
    }

    /**
     * @brief Formats the run mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printRunMode(StringBuilder &str) const {
        printRouted(str, Routing::Target::RunMode);
        str(Types::runModeName(runMode()));
    }

    // firstStep

    /**
     * @brief Returns the first step.
     *
     * @return Zero-based inclusive first step of the active sequence range.
     */
    int firstStep() const {
        return _firstStep.get(isRouted(Routing::Target::FirstStep));
    }

    /**
     * @brief Sets the first step.
     *
     * @param[in] firstStep New zero-based inclusive first step to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    /**
     * @brief Adjusts the first step from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editFirstStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    /**
     * @brief Formats the first step into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printFirstStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::FirstStep);
        str(TXT_MODEL_PRINT_FIRST_STEP, firstStep() + 1);
    }

    // lastStep

    /**
     * @brief Returns the last step.
     *
     * @return Zero-based inclusive last step of the active sequence range.
     */
    int lastStep() const {
        // make sure last step is always >= first step even if stored value is invalid (due to routing changes)
        return std::max(firstStep(), int(_lastStep.get(isRouted(Routing::Target::LastStep))));
    }

    /**
     * @brief Sets the last step.
     *
     * @param[in] lastStep New zero-based inclusive last step to store or apply.
     * @param[in] routed Whether to access the routed value (`true`) instead of the base value (`false`).
     */
    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1), routed);
    }

    /**
     * @brief Adjusts the last step from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editLastStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::LastStep)) {
            setLastStep(lastStep() + value);
        }
    }

    /**
     * @brief Formats the last step into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printLastStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::LastStep);
        str(TXT_MODEL_PRINT_LAST_STEP, lastStep() + 1);
    }

    // steps

    /**
     * @brief Returns the steps.
     *
     * @return Reference to the steps.
     */
    const StepArray &steps() const { return _steps; }
          /**
           * @brief Returns the steps.
           *
           * @return Reference to the steps.
           */
          StepArray &steps()       { return _steps; }

    /**
     * @brief Advances the NoteSequence by one logical step.
     *
     * @param[in] index Zero-based step index.
     *
     * @return Reference to the step.
     */
    const Step &step(int index) const { return _steps[index]; }
          /**
           * @brief Advances the NoteSequence by one logical step.
           *
           * @param[in] index Zero-based step index.
           *
           * @return Reference to the step.
           */
          Step &step(int index)       { return _steps[index]; }

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
     * @brief Constructs a NoteSequence instance.
     */
    NoteSequence() { clear(); }

    // Minimal public wrapper to allow tracks to assign per-sequence trackIndex.
    // For internal use only; forwards to the private setTrackIndex implementation.
    /**
     * @brief Sets the track index public.
     *
     * @param[in] trackIndex Zero-based track index.
     */
    void setTrackIndexPublic(int trackIndex) { setTrackIndex(trackIndex); }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();
    /**
     * @brief Clears the addressed sequence steps.
     */
    void clearSteps();

    /**
     * @brief Reports whether edited.
     *
     * @return `true` if edited; otherwise `false`.
     */
    bool isEdited() const;

    /**
     * @brief Sets the gates.
     *
     * @param[in] gates Gate bit mask or gate-state collection consumed by the operation.
     */
    void setGates(std::initializer_list<int> gates);
    /**
     * @brief Sets the notes.
     *
     * @param[in] notes Array or collection of note values consumed by the operation.
     */
    void setNotes(std::initializer_list<int> notes);

    /**
     * @brief Shifts steps.
     *
     * @param[in] selected Selection state or selection set applied by the operation.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     */
    void shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction);

    /**
     * @brief Duplicates steps.
     */
    void duplicateSteps();

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
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    /**
     * @brief Offsets first and last step.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void offsetFirstAndLastStep(int value) {
        value = clamp(value, -firstStep(), CONFIG_STEP_COUNT - 1 - lastStep());
        if (value > 0) {
            editLastStep(value, false);
            editFirstStep(value, false);
        } else {
            editFirstStep(value, false);
            editLastStep(value, false);
        }
    }

    /**
     * @brief Zero-based track index; a negative/sentinel value represents no selection where applicable.
     */
    int8_t _trackIndex = -1; ///< Zero-based track index; a negative/sentinel value represents no selection where applicable.
    Routable<int8_t> _scale; ///< Base and routed scale setting, resolved through the routing system when queried.
    Routable<int8_t> _rootNote; ///< Base and routed root note setting, resolved through the routing system when queried.
    Routable<uint16_t> _divisor; ///< Configured timing divisor in the owning clock/sequence domain.
    uint8_t _resetMeasure; ///< Measure interval at which sequence position is reset; zero disables measure-based reset.
    Routable<Types::RunMode> _runMode; ///< Base and routed traversal mode controlling how note steps advance.
    Routable<uint8_t> _firstStep; ///< Base and routed first step setting, resolved through the routing system when queried.
    /**
     * @brief Most recently observed step.
     */
    Routable<uint8_t> _lastStep; ///< Base and routed inclusive end step of the active note sequence range.

    StepArray _steps; ///< Owned fixed-size collection of steps.

    uint8_t _edited; ///< Nonzero when step content differs from the sequence baseline used by edit-state tracking.

    friend class NoteTrack;
};
