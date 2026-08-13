/**
 * @file EuclideanGenerator.h
 * @brief Declares the EuclideanGenerator component used by the sequencer generator.
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

#include "Generator.h"
#include "Rhythm.h"

#include "core/math/Math.h"

/**
 * @brief Generates evenly distributed Euclidean gate patterns for the selected step count.
 */
class EuclideanGenerator : public Generator {
public:
    /**
     * @brief Enumerates the supported param values.
     */
    enum class Param {
        Steps, ///< Selects the steps param.
        Beats, ///< Selects the beats param.
        Offset, ///< Selects the offset param.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Stores the configurable parameters consumed by the enclosing algorithm.
     */
    struct Params {
        /**
         * @brief Runtime value representing steps.
         */
        uint8_t steps = 16; ///< Step storage/count associated with this object.
        /**
         * @brief Runtime value representing beats.
         */
        uint8_t beats = 4; ///< Beat count represented by this object.
        /**
         * @brief Offset applied to  in this type's timing/coordinate domain.
         */
        uint8_t offset = 0; ///< Offset applied to  in this type's timing/coordinate domain.
    };

    /**
     * @brief Constructs a EuclideanGenerator instance.
     *
     * @param[in] builder String/data builder that receives the generated representation.
     * @param[in] params Parameter set controlling the generated pattern or game object.
     */
    EuclideanGenerator(SequenceBuilder &builder, Params &params);

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const override { return Mode::Euclidean; }

    /**
     * @brief Returns the param count.
     *
     * @return Number of param entries represented by the object.
     */
    int paramCount() const override { return int(Param::Last); }
    /**
     * @brief Returns the display name for param.
     *
     * @param[in] index Zero-based param name index.
     *
     * @return Pointer to the param name; `nullptr` when no value is available.
     */
    const char *paramName(int index) const override;
    /**
     * @brief Adjusts the param from a UI edit delta.
     *
     * @param[in] index Zero-based param index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editParam(int index, int value, bool shift) override;
    /**
     * @brief Formats the param into the supplied string builder/output.
     *
     * @param[in] index Zero-based param index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void printParam(int index, StringBuilder &str) const override;

    /**
     * @brief Initializes the EuclideanGenerator and its runtime resources.
     */
    void init() override;
    /**
     * @brief Updates the EuclideanGenerator for the current service cycle.
     */
    void update() override;

    // steps

    /**
     * @brief Returns the steps.
     *
     * @return Number of steps represented by this object.
     */
    int steps() const { return _params.steps; }
    /**
     * @brief Sets the steps.
     *
     * @param[in] steps Steps consumed by `setSteps()`.
     */
    void setSteps(int steps) { _params.steps = clamp(steps, 1, CONFIG_STEP_COUNT); }

    // beats

    /**
     * @brief Returns the beats.
     *
     * @return Time-signature numerator in beats per measure.
     */
    int beats() const { return _params.beats; }
    /**
     * @brief Sets the beats.
     *
     * @param[in] beats Number of beats per measure represented by the time signature.
     */
    void setBeats(int beats) { _params.beats = clamp(beats, 1, CONFIG_STEP_COUNT); }

    // offset

    /**
     * @brief Returns the offset.
     *
     * @return Configured curve-track offset.
     */
    int offset() const { return _params.offset; }
    /**
     * @brief Sets the offset.
     *
     * @param[in] offset Offset applied in the domain defined by the operation.
     */
    void setOffset(int offset) { _params.offset = clamp(offset, 0, CONFIG_STEP_COUNT - 1); }

    // pattern

    /**
     * @brief Returns the pattern.
     *
     * @return Reference to the pattern.
     */
    const Rhythm::Pattern &pattern() const { return _pattern; }

private:
    /**
     * @brief Params, in milliseconds.
     */
    Params &_params; ///< Mutable generator parameter set supplied by the owning generator page/model.
    Rhythm::Pattern _pattern; ///< Euclidean rhythm bit pattern generated from the current pulse and step counts.
};
