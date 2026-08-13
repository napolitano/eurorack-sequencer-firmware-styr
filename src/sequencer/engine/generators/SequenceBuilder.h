/**
 * @file SequenceBuilder.h
 * @brief Declares the SequenceBuilder component used by the sequencer generator.
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

#include "model/NoteSequence.h"
#include "model/CurveSequence.h"

/**
 * @brief Defines the interface used by generators to write values into sequence layers.
 */
class SequenceBuilder {
public:
    /**
     * @brief Restores the previously saved or snapshotted state.
     */
    virtual void revert() = 0;

    // original sequence

    /**
     * @brief Returns the original length.
     *
     * @return Sequence length captured before generator edits are applied.
     */
    virtual int originalLength() const = 0;
    /**
     * @brief Returns original value.
     *
     * @param[in] index Zero-based original index.
     *
     * @return Original/unmodified value.
     */
    virtual float originalValue(int index) const = 0;

    // edit sequence

    /**
     * @brief Returns the length.
     *
     * @return Number of length represented by this object.
     */
    virtual int length() const = 0;
    /**
     * @brief Sets the length.
     *
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    virtual void setLength(int length) = 0;

    /**
     * @brief Returns value.
     *
     * @param[in] index Zero-based value index.
     *
     * @return Value carried by this event or model field.
     */
    virtual float value(int index) const = 0;
    /**
     * @brief Sets the value.
     *
     * @param[in] index Zero-based value index.
     * @param[in] value New value to store or apply.
     */
    virtual void setValue(int index, float value) = 0;

    /**
     * @brief Clears the addressed sequence steps.
     */
    virtual void clearSteps() = 0;
    /**
     * @brief Copies step.
     *
     * @param[in] fromIndex Zero-based from index.
     * @param[in] toIndex Zero-based to index.
     */
    virtual void copyStep(int fromIndex, int toIndex) = 0;

    /**
     * @brief Clears layer.
     */
    virtual void clearLayer() = 0;
};

/**
 * @brief Writes generated values into the selected layer/range of a concrete sequence type.
 */
template<typename T>
/**
 * @brief Builds and transforms sequence values while retaining the source sequence length.
 */
class SequenceBuilderImpl : public SequenceBuilder {
public:
    /**
     * @brief Constructs a SequenceBuilderImpl instance.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     */
    SequenceBuilderImpl(T &sequence, typename T::Layer layer) :
        /**
         * @brief Returns the edit.
         */
        _edit(sequence),
        /**
         * @brief Returns the original.
         */
        _original(sequence),
        /**
         * @brief Returns the layer.
         */
        _layer(layer),
        /**
         * @brief Returns the range.
         */
        _range(T::layerRange(layer)),
        /**
         * @brief Returns the default.
         */
        _default(T::layerDefaultValue(layer))
    {}

    /**
     * @brief Restores the previously saved or snapshotted state.
     */
    void revert() override {
        _edit = _original;
    }

    /**
     * @brief Returns the original length.
     *
     * @return Sequence length captured before generator edits are applied.
     */
    int originalLength() const override {
        return _original.lastStep() - _original.firstStep() + 1;
    }

    /**
     * @brief Returns original value.
     *
     * @param[in] index Zero-based original index.
     *
     * @return Original/unmodified value.
     */
    float originalValue(int index) const override {
        int layerValue = _original.step(_original.firstStep() + index).layerValue(_layer);
        return float(layerValue - _range.min) / (_range.max - _range.min);
    }

    /**
     * @brief Returns the length.
     *
     * @return Number of length represented by this object.
     */
    int length() const override {
        return _edit.lastStep() - _edit.firstStep() + 1;
    }

    /**
     * @brief Sets the length.
     *
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    void setLength(int length) override {
        _edit.setFirstStep(0);
        _edit.setLastStep(length - 1);
    }

    /**
     * @brief Returns value.
     *
     * @param[in] index Zero-based value index.
     *
     * @return Value carried by this event or model field.
     */
    float value(int index) const override {
        int layerValue = _edit.step(_edit.firstStep() + index).layerValue(_layer);
        return float(layerValue - _range.min) / (_range.max - _range.min);
    }

    /**
     * @brief Sets the value.
     *
     * @param[in] index Zero-based value index.
     * @param[in] value New value to store or apply.
     */
    void setValue(int index, float value) override {
        int layerValue = std::round(value * (_range.max - _range.min) + _range.min);
        _edit.step(_edit.firstStep() + index).setLayerValue(_layer, layerValue);
    }

    /**
     * @brief Clears the addressed sequence steps.
     */
    void clearSteps() override {
        _edit.clearSteps();
    }

    /**
     * @brief Copies step.
     *
     * @param[in] fromIndex Zero-based from index.
     * @param[in] toIndex Zero-based to index.
     */
    void copyStep(int fromIndex, int toIndex) override {
        _edit.step(_edit.firstStep() + toIndex) = _original.step(_original.firstStep() + fromIndex);
    }

    /**
     * @brief Clears layer.
     */
    void clearLayer() override {
        for (auto &step : _edit.steps()) {
            step.setLayerValue(_layer, _default);
        }
    }

private:
    /**
     * @brief Reference to edit owned by another component.
     */
    T &_edit; ///< Reference to edit owned by another component.
    T _original; ///< Original sequence data retained so generator edits can be previewed or rolled back before commit.
    typename T::Layer _layer; ///< Sequence layer currently written by this builder pass.
    Types::LayerRange _range; ///< Inclusive layer range over which this builder applies generated values.
    /**
     * @brief Runtime value representing default.
     */
    int _default;; ///< Default value used when no explicit value is present.
};

typedef SequenceBuilderImpl<NoteSequence> NoteSequenceBuilder;
typedef SequenceBuilderImpl<CurveSequence> CurveSequenceBuilder;
