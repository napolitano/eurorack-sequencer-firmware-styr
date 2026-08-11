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

class Generator {
public:
    enum class Mode {
        InitLayer,
        Euclidean,
        Random,
#ifdef CONFIG_ACID_BASS_GENERATOR
        AcidBassline,
#endif
        Last
    };

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

    Generator(SequenceBuilder &builder) :
        _builder(builder)
    {}

    virtual Mode mode() const = 0;
    const char *name() const { return modeName(mode()); }

    // parameters

    virtual int paramCount() const = 0;
    virtual const char *paramName(int index) const = 0;
    virtual void editParam(int index, int value, bool shift) = 0;
    virtual void printParam(int index, StringBuilder &str) const = 0;

    virtual void init() {}

    virtual void revert() {
        _builder.revert();
    }

    virtual void update() = 0;

    static Generator *execute(Generator::Mode mode, SequenceBuilder &builder);

protected:
    SequenceBuilder &_builder;
};
