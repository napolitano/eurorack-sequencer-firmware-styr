/**
 * @file RandomGenerator.h
 * @brief Declares the RandomGenerator component used by the sequencer generator.
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

#include "core/math/Math.h"

/**
 * @brief Generates randomized sequence values with configurable smoothing and bias.
 */
class RandomGenerator : public Generator {
public:
    /**
     * @brief Enumerates the supported param values.
     */
    enum class Param {
        Seed, ///< Selects the seed param.
        Smooth, ///< Selects the smooth param.
        Bias, ///< Selects the bias param.
        Scale, ///< Selects the scale param.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Stores the configurable parameters consumed by the enclosing algorithm.
     */
    struct Params {
        /**
         * @brief Runtime value representing seed.
         */
        uint16_t seed = 0; ///< Random-number generator seed/state.
        /**
         * @brief Runtime value representing smooth.
         */
        uint8_t smooth = 0; ///< Smoothing amount/state used by the generator/curve.
        /**
         * @brief Runtime value representing bias.
         */
        int8_t bias = 0; ///< Bias applied by the generator/selection heuristic.
        /**
         * @brief Runtime value representing scale.
         */
        uint8_t scale = 10; ///< Scale selection/definition stored by this state.
    };

    /**
     * @brief Constructs a RandomGenerator instance.
     *
     * @param[in] builder String/data builder that receives the generated representation.
     * @param[in] params Parameter set controlling the generated pattern or game object.
     */
    RandomGenerator(SequenceBuilder &builder, Params &params);

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const override { return Mode::Random; }

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
     * @brief Initializes the RandomGenerator and its runtime resources.
     */
    void init() override;
    /**
     * @brief Updates the RandomGenerator for the current service cycle.
     */
    void update() override;

    // seed

    /**
     * @brief Returns the seed.
     *
     * @return Current pseudo-random generator seed.
     */
    int seed() const { return _params.seed; }
    /**
     * @brief Sets the seed.
     *
     * @param[in] seed Seed used to initialize the pseudo-random number generator.
     */
    void setSeed(int seed) { _params.seed = clamp(seed, 0, 1000); }

    // smooth

    /**
     * @brief Returns the smooth.
     *
     * @return Configured random-generator smoothing amount.
     */
    int smooth() const { return _params.smooth; }
    /**
     * @brief Sets the smooth.
     *
     * @param[in] smooth New generator smoothing amount to store or apply; values are clamped to `0`..`10`.
     */
    void setSmooth(int smooth) { _params.smooth = clamp(smooth, 0, 10); }

    // bias

    /**
     * @brief Returns the bias.
     *
     * @return Configured random-generator value bias.
     */
    int bias() const { return _params.bias; }
    /**
     * @brief Sets the bias.
     *
     * @param[in] bias New generator bias amount to store or apply; values are clamped to `-10`..`10`.
     */
    void setBias(int bias) { _params.bias = clamp(bias, -10, 10); }

    // scale

    /**
     * @brief Returns the scale.
     *
     * @return Configured scale selection.
     */
    int scale() const { return _params.scale; }
    /**
     * @brief Sets the scale.
     *
     * @param[in] scale Scale definition used for quantization or display.
     */
    void setScale(int scale) { _params.scale = clamp(scale, 0, 100); }

    // pattern

    /**
     * @brief Returns the pattern.
     *
     * @return Reference to the pattern.
     */
    const GeneratorPattern &pattern() const { return _pattern; }

private:
    /**
     * @brief Params, in milliseconds.
     */
    Params &_params; ///< Mutable generator parameter set supplied by the owning generator page/model.
    GeneratorPattern _pattern; ///< Generated pattern data retained between generator parameter edits and commit.
};
