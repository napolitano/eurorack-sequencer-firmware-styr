/**
 * @file GeneratorSelectListModel.h
 * @brief Declares the GeneratorSelectListModel component used by the sequencer UI.
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

#include "ListModel.h"

#include "engine/generators/Generator.h"

class GeneratorSelectListModel : public ListModel {
public:
    virtual int rows() const override {
        return int(Generator::Mode::Last);
    }

    virtual int columns() const override {
        return 1;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str(Generator::modeName(Generator::Mode(row)));
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
    }

};
