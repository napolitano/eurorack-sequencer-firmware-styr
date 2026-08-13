/**
 * @file ContextMenu.h
 * @brief Declares the ContextMenu component used by the sequencer UI page.
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

#include "ui/model/ContextMenuModel.h"

#include <functional>

/**
 * @brief Implements a fixed-item context menu backed by action and enable callbacks.
 */
class ContextMenu : public ContextMenuModel {
public:
    typedef std::function<void(int)> ActionCallback;
    typedef std::function<bool(int)> ItemEnabledCallback;

    /**
     * @brief Constructs a ContextMenu instance.
     */
    ContextMenu() = default;

    /**
     * @brief Constructs a context menu backed by a fixed item array and callbacks.
     *
     * @param[in] items Array of menu items that remains valid for the lifetime of this `ContextMenu`.
     * @param[in] itemCount Number of entries in `items`.
     * @param[in] actionCallback Callback invoked with the selected zero-based item index.
     * @param[in] itemEnabledCallback Predicate used to determine whether an item is currently selectable.
     */
    ContextMenu(
        const Item items[],
        int itemCount,
        ActionCallback actionCallback,
        ItemEnabledCallback itemEnabledCallback = [] (int) { return true; }
    /**
     * @brief Computes the items result.
     *
     * @note Includes an unnamed `items` input parameter as declared by the inherited/interface signature.
     *
     * @return Reference to the stored item collection.
     */
    ) :
        _items(items),
        _itemCount(itemCount),
        _actionCallback(actionCallback),
        _itemEnabledCallback(itemEnabledCallback)
    {
    }

    /**
     * @brief Returns the action callback.
     *
     * @return Reference to the action callback.
     */
    const ActionCallback &actionCallback() const { return _actionCallback; }

private:
    /**
     * @brief Returns the item count.
     *
     * @return Number of item entries represented by the object.
     */
    virtual int itemCount() const override {
        return _itemCount;
    }

    /**
     * @brief Returns item.
     *
     * @param[in] index Zero-based item index.
     *
     * @return Reference to the item.
     */
    virtual const ContextMenuModel::Item &item(int index) const override {
        return _items[index];
    }

    /**
     * @brief Returns item enabled.
     *
     * @param[in] index Zero-based item enabled index.
     *
     * @return `true` if item enabled; otherwise `false`.
     */
    virtual bool itemEnabled(int index) const override {
        return _itemEnabledCallback(index);
    }

    /**
     * @brief Non-owning menu-item array supplied by the caller.
     */
    const Item *_items; ///< Non-owning pointer to the fixed menu-item array supplied at construction.
    /**
     * @brief Number of menu items available in this context menu.
     */
    int _itemCount; ///< Number of menu items available in this context menu.
    ActionCallback _actionCallback; ///< Callback invoked for action activity.
    ItemEnabledCallback _itemEnabledCallback; ///< Callback invoked for item enabled activity.
};
