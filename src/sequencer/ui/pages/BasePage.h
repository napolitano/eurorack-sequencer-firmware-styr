/**
 * @file BasePage.h
 * @brief Declares the BasePage component used by the sequencer UI page.
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

#include "ui/MessageManager.h"
#include "ui/Page.h"
#include "ui/PageManager.h"
#include "ui/Key.h"
#include "ui/pages/ContextMenu.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include <cstdint>

/**
 * @brief Stores the fields required to represent page context.
 */
struct PageContext {
    /**
     * @brief Reference to message manager owned by another component.
     */
    MessageManager &messageManager; ///< Reference to message manager owned by another component.
    /**
     * @brief Reference to page key state owned by another component.
     */
    KeyState &pageKeyState; ///< Reference to page key state owned by another component.
    /**
     * @brief Reference to global key state owned by another component.
     */
    KeyState &globalKeyState; ///< Reference to global key state owned by another component.
    /**
     * @brief Persistent sequencer model used by this component.
     */
    Model &model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &engine; ///< Reference to engine owned by another component.

    ContextMenu contextMenu; ///< Context-menu model/state for the current page.
};

/**
 * @brief Implements the base page user-interface page.
 */
class BasePage : public Page {
public:
    /**
     * @brief Constructs a BasePage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    BasePage(PageManager &manager, PageContext &context);


protected:
    /**
     * @brief Displays a transient user message.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     * @param[in] duration Duration in the unit defined by the owning API.
     */
    void showMessage(const char *text, uint32_t duration = 1000);
    /**
     * @brief Shows context menu.
     *
     * @param[in] contextMenu Context-menu model opened by the page.
     */
    void showContextMenu(const ContextMenu &contextMenu);

    /**
     * @brief Returns the page key state.
     *
     * @return Reference to the page key state.
     */
    const KeyState &pageKeyState() const { return _context.pageKeyState; }
    /**
     * @brief Returns the global key state.
     *
     * @return Reference to the global key state.
     */
    const KeyState &globalKeyState() const { return _context.globalKeyState; }

    /**
     * @brief Reference to context owned by another component.
     */
    PageContext &_context; ///< Reference to context owned by another component.
    /**
     * @brief Persistent sequencer model used by this component.
     */
    Model &_model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
    /**
     * @brief Reference to engine owned by another component.
     */
    Engine &_engine; ///< Reference to engine owned by another component.
};
