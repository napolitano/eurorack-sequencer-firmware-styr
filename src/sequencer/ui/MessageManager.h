/**
 * @file MessageManager.h
 * @brief Declares the MessageManager component used by the sequencer UI.
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

#include "os/os.h"

#include "core/gfx/Canvas.h"

#include <cstdint>

/**
 * @brief Queues and displays transient user-interface messages.
 */
class MessageManager {
public:
    /**
     * @brief Constructs a MessageManager instance.
     */
    MessageManager();

    /**
     * @brief Displays a transient user message.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     * @param[in] duration Duration in the unit defined by the owning API.
     */
    void showMessage(const char *text, uint32_t duration = 1000);

    /**
     * @brief Updates the MessageManager for the current service cycle.
     */
    void update();

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    /**
     * @brief UI value representing text.
     */
    char _text[64]; ///< Null-terminated text buffer owned by the UI component.
    /**
     * @brief UI value representing timeout.
     */
    uint32_t _timeout = 0; ///< Remaining UI message lifetime in scheduler ticks; zero means no timeout is pending.

    os::Mutex _mutex; ///< Mutex protecting concurrent access to the associated `MessageManager` state.
};
