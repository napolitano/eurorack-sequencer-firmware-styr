/**
 * @file Generator.h
 * @brief Declares the Generator component used by the sequencer generator.
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

#include "SequenceBuilder.h"

#include "core/utils/StringBuilder.h"

#include <array>

typedef std::array<uint8_t, CONFIG_STEP_COUNT> GeneratorPattern;

/**
 * @brief Defines the common parameter and generation interface for sequence generators.
 */
class Generator {
public:
    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode {
        InitLayer, ///< Selects the init layer mode.
        Euclidean, ///< Selects the euclidean mode.
        Random, ///< Selects the random mode.
#ifdef CONFIG_ACID_BASS_GENERATOR
        AcidBassline, ///< Selects the acid bassline mode.
#endif
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the mode name; `nullptr` when no value is available.
     */
    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::InitLayer:   return "Init Layer";
        case Mode::Euclidean:   return "Euclidean";
        case Mode::Random:      return "Random";
#ifdef CONFIG_ACID_BASS_GENERATOR
        case Mode::AcidBassline: return "Acid Bassline";
#endif
        case Mode::Last:        break;
        }
        return nullptr;
    }

    /**
     * @brief Constructs a Generator instance.
     *
     * @param[in] builder String/data builder that receives the generated representation.
     */
    Generator(SequenceBuilder &builder) :
        /**
         * @brief Returns the builder.
         */
        _builder(builder)
    {}

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    virtual Mode mode() const = 0;
    /**
     * @brief Returns the name.
     *
     * @return Pointer to the name; `nullptr` when no value is available.
     */
    const char *name() const { return modeName(mode()); }

    // parameters

    /**
     * @brief Returns the param count.
     *
     * @return Number of param entries represented by the object.
     */
    virtual int paramCount() const = 0;
    /**
     * @brief Returns the display name for param.
     *
     * @param[in] index Zero-based param name index.
     *
     * @return Pointer to the param name; `nullptr` when no value is available.
     */
    virtual const char *paramName(int index) const = 0;
    /**
     * @brief Adjusts the param from a UI edit delta.
     *
     * @param[in] index Zero-based param index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    virtual void editParam(int index, int value, bool shift) = 0;
    /**
     * @brief Formats the param into the supplied string builder/output.
     *
     * @param[in] index Zero-based param index.
     * @param[out] str String builder that receives the formatted representation.
     */
    virtual void printParam(int index, StringBuilder &str) const = 0;

    /**
     * @brief Initializes the Generator and its runtime resources.
     */
    virtual void init() {}

    /**
     * @brief Restores the previously saved or snapshotted state.
     */
    virtual void revert() {
        _builder.revert();
    }

    /**
     * @brief Updates the Generator for the current service cycle.
     */
    virtual void update() = 0;

    /**
     * @brief Returns execute.
     *
     * @param[in] mode Mode to select.
     * @param[in] builder String/data builder that receives the generated representation.
     *
     * @return Pointer to the execute; `nullptr` when no value is available.
     */
    static Generator *execute(Generator::Mode mode, SequenceBuilder &builder);

protected:
    /**
     * @brief Reference to builder owned by another component.
     */
    SequenceBuilder &_builder; ///< Reference to builder owned by another component.
};
