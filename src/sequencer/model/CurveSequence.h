/**
 * @file CurveSequence.h
 * @brief Declares the CurveSequence component used by the sequencer model.
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
#include "Curve.h"
#include "Routing.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <initializer_list>

/**
 * @brief Stores and manipulates curve sequence model data.
 */
class CurveSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<6> Shape;
    typedef UnsignedValue<4> ShapeVariationProbability;
    typedef UnsignedValue<8> Min;
    typedef UnsignedValue<8> Max;
    typedef UnsignedValue<4> Gate;
    typedef UnsignedValue<3> GateProbability;

    /**
     * @brief Enumerates the supported layer values.
     */
    enum class Layer {
        Shape, ///< Selects the shape layer.
        ShapeVariation, ///< Selects the shape variation layer.
        ShapeVariationProbability, ///< Selects the shape variation probability layer.
        Min, ///< Selects the min layer.
        Max, ///< Selects the max layer.
        Gate, ///< Selects the gate layer.
        GateProbability, ///< Selects the gate probability layer.
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
        case Layer::Shape:                      return TXT_MODEL_SHAPE;
        case Layer::ShapeVariation:             return TXT_MODEL_SHAPE_VARIATION;
        case Layer::ShapeVariationProbability:  return TXT_MODEL_SHAPE_VARIATION_PROBABILITY;
        case Layer::Min:                        return TXT_MODEL_MINIMUM;
        case Layer::Max:                        return TXT_MODEL_MAXIMUM;
        case Layer::Gate:                       return TXT_MODEL_GATE;
        case Layer::GateProbability:            return TXT_MODEL_GATE_PROBABILITY;
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
     * @brief Stores and manipulates step model data.
     */
    class Step {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // shape

        /**
         * @brief Returns the shape.
         *
         * @return Configured curve shape.
         */
        int shape() const { return _data0.shape; }
        /**
         * @brief Sets the shape.
         *
         * @param[in] shape Curve/geometric shape selected by the operation.
         */
        void setShape(int shape) {
            _data0.shape = clamp(shape, 0, int(Curve::Last) - 1);
        }

        // shapeVariation

        /**
         * @brief Returns the shape variation.
         *
         * @return Configured alternate curve shape.
         */
        int shapeVariation() const { return _data0.shapeVariation; }
        /**
         * @brief Sets the shape variation.
         *
         * @param[in] shapeVariation Shape variation consumed by `setShapeVariation()`.
         */
        void setShapeVariation(int shapeVariation) {
            _data0.shapeVariation = clamp(shapeVariation, 0, int(Curve::Last) - 1);
        }

        // shapeVariationProbability

        /**
         * @brief Returns the shape variation probability.
         *
         * @return Probability of applying the alternate curve shape.
         */
        int shapeVariationProbability() const { return _data0.shapeVariationProbability; }
        /**
         * @brief Sets the shape variation probability.
         *
         * @param[in] shapeVariationProbability New shape-variation probability in the 0..100 range to store or apply.
         */
        void setShapeVariationProbability(int shapeVariationProbability) {
            _data0.shapeVariationProbability = clamp(shapeVariationProbability, 0, 8);
        }

        // min

        /**
         * @brief Returns the min.
         *
         * @return Configured minimum value.
         */
        int min() const { return _data0.min; }
        /**
         * @brief Sets the min.
         *
         * @param[in] min Minimum accepted or represented value.
         */
        void setMin(int min) {
            _data0.min = Min::clamp(min);
            _data0.max = std::max(max(), this->min());
        }

        /**
         * @brief Returns the min normalized.
         *
         * @return Normalized minimum curve value.
         */
        float minNormalized() const { return float(min()) / Min::Max; }
        /**
         * @brief Sets the min normalized.
         *
         * @param[in] min Minimum accepted or represented value.
         */
        void setMinNormalized(float min) {
            setMin(int(std::round(min * Min::Max)));
        }

        // max

        /**
         * @brief Returns the max.
         *
         * @return Configured maximum value.
         */
        int max() const { return _data0.max; }
        /**
         * @brief Sets the max.
         *
         * @param[in] max Maximum accepted or represented value.
         */
        void setMax(int max) {
            _data0.max = Max::clamp(max);
            _data0.min = std::min(min(), this->max());
        }

        /**
         * @brief Returns the max normalized.
         *
         * @return Normalized maximum curve value.
         */
        float maxNormalized() const { return float(max()) / Max::Max; }
        /**
         * @brief Sets the max normalized.
         *
         * @param[in] max Maximum accepted or represented value.
         */
        void setMaxNormalized(float max) {
            setMax(int(std::round(max * Max::Max)));
        }

        // gate

        /**
         * @brief Returns the gate.
         *
         * @return Configured curve-step gate state.
         */
        int gate() const { return _data1.gate; }
        /**
         * @brief Sets the gate.
         *
         * @param[in] gate Gate consumed by `setGate()`.
         */
        void setGate(int gate) {
            _data1.gate = Gate::clamp(gate);
        }

        // gateProbability

        /**
         * @brief Returns the gate probability.
         *
         * @return Gate probability in the model-defined 0..100 range.
         */
        int gateProbability() const { return _data1.gateProbability; }
        /**
         * @brief Sets the gate probability.
         *
         * @param[in] gateProbability New gate probability in the 0..100 range to store or apply.
         */
        void setGateProbability(int gateProbability) {
            _data1.gateProbability = GateProbability::clamp(gateProbability);
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
            BitField<uint32_t, 0, Shape::Bits> shape;
            BitField<uint32_t, 6, Shape::Bits> shapeVariation;
            BitField<uint32_t, 12, ShapeVariationProbability::Bits> shapeVariationProbability;
            BitField<uint32_t, 16, Min::Bits> min;
            BitField<uint32_t, 24, Max::Bits> max;
        } _data0;
        /**
         * @brief Raw encoded bytes that make up the value/message.
         */
        union {
            uint16_t raw;
            BitField<uint16_t, 0, Gate::Bits> gate;
            BitField<uint16_t, 4, GateProbability::Bits> gateProbability;
            // 9 bits left
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
     * @brief Advances the CurveSequence by one logical step.
     *
     * @param[in] index Zero-based step index.
     *
     * @return Reference to the step.
     */
    const Step &step(int index) const { return _steps[index]; }
          /**
           * @brief Advances the CurveSequence by one logical step.
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
     * @brief Constructs a CurveSequence instance.
     */
    CurveSequence() { clear(); }

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
     * @brief Sets the shapes.
     *
     * @param[in] shapes Shapes consumed by `setShapes()`.
     */
    void setShapes(std::initializer_list<int> shapes);

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
    Types::VoltageRange _range; ///< Configured CV voltage range.
    Routable<uint16_t> _divisor; ///< Configured timing divisor in the owning clock/sequence domain.
    uint8_t _resetMeasure; ///< Measure interval at which sequence position is reset; zero disables measure-based reset.
    Routable<Types::RunMode> _runMode; ///< Base and routed traversal mode controlling how curve steps advance.
    Routable<uint8_t> _firstStep; ///< Base and routed first step setting, resolved through the routing system when queried.
    /**
     * @brief Most recently observed step.
     */
    Routable<uint8_t> _lastStep; ///< Base and routed inclusive end step of the active curve sequence range.

    StepArray _steps; ///< Owned fixed-size collection of steps.

    friend class CurveTrack;
};
