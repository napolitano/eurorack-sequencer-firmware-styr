/**
 * @file EuclideanGenerator.cpp
 * @brief Implements the EuclideanGenerator component used by the sequencer generator.
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
#include "EuclideanGenerator.h"

EuclideanGenerator::EuclideanGenerator(SequenceBuilder &builder, Params& params) :
    Generator(builder),
    _params(params)
{
    update();
}

const char *EuclideanGenerator::paramName(int index) const {
    switch (Param(index)) {
    case Param::Steps:  return TXT_MENU_STEPS;
    case Param::Beats:  return TXT_MENU_BEATS;
    case Param::Offset: return TXT_MENU_OFFSET;
    case Param::Last:   break;
    }
    return nullptr;
}

void EuclideanGenerator::editParam(int index, int value, bool shift) {
    switch (Param(index)) {
    case Param::Steps:  setSteps(steps() + value); break;
    case Param::Beats:  setBeats(beats() + value); break;
    case Param::Offset: setOffset(offset() + value); break;
    case Param::Last:   break;
    }
}

void EuclideanGenerator::printParam(int index, StringBuilder &str) const {
    switch (Param(index)) {
    case Param::Steps:  str("%d", steps()); break;
    case Param::Beats:  str("%d", beats()); break;
    case Param::Offset: str("%d", offset()); break;
    case Param::Last:   break;
    }
}

void EuclideanGenerator::init()
{
    _params = Params();
    update();
}

void EuclideanGenerator::update()  {
    _pattern = Rhythm::euclidean(_params.beats, _params.steps).shifted(_params.offset);

    _builder.setLength(_params.steps);

    for (size_t i = 0; i < CONFIG_STEP_COUNT; ++i) {
        _builder.setValue(i, _pattern[i % _pattern.size()] ? 1.f : 0.f);
    }
}
