/**
 * @file ContextMenuModel.h
 * @brief Declares the ContextMenuModel component used by the sequencer UI.
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

#include "core/utils/StringBuilder.h"

class ContextMenuModel {
public:
    struct Item {
        const char *title;
        // uint8_t icon;
    };

    virtual int itemCount() const = 0;

    virtual const Item &item(int index) const = 0;
    virtual bool itemEnabled(int index) const = 0;
};
