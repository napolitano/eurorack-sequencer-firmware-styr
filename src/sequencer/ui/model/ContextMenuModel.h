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

/**
 * @brief Stores and manipulates context menu model model data.
 */
class ContextMenuModel {
public:
    /**
     * @brief Stores and manipulates item model data.
     */
    struct Item {
        /**
         * @brief Pointer to title; `nullptr` denotes that no object/resource is assigned.
         */
        const char *title; ///< Pointer to title; `nullptr` denotes that no object/resource is assigned.
        // uint8_t icon;
    };

    /**
     * @brief Returns the item count.
     *
     * @return Number of item entries represented by the object.
     */
    virtual int itemCount() const = 0;

    /**
     * @brief Returns item.
     *
     * @param[in] index Zero-based item index.
     *
     * @return Reference to the item.
     */
    virtual const Item &item(int index) const = 0;
    /**
     * @brief Returns item enabled.
     *
     * @param[in] index Zero-based item enabled index.
     *
     * @return `true` if item enabled; otherwise `false`.
     */
    virtual bool itemEnabled(int index) const = 0;
};
