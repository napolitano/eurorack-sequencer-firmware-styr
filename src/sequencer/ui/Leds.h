/**
 * @file Leds.h
 * @brief Declares the Leds component used by the sequencer UI.
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

#include "core/Debug.h"

#include <array>
#include <bitset>
#include <utility>

#include <cstdint>

/**
 * @brief Maintains and renders the front-panel LED state.
 */
class Leds {
public:
    typedef std::array<std::pair<uint8_t, uint8_t>, CONFIG_BLM_ROWS * CONFIG_BLM_COLS_LED> LedArray;

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _array.fill({ 0, 0 });
        _mask.reset();
    }

    /**
     * @brief Sets the dimmed.
     *
     * @param[in] index Zero-based dimmed index.
     * @param[in] red Red color-channel intensity.
     * @param[in] green Green color-channel intensity.
     */
    void setDimmed(int index, uint8_t red, uint8_t green) {
        ASSERT(index >= 0 && index < int(_array.size()), "invalid led index");
        if (_mask[index]) {
            return;
        }
        _array[index] = { red, green };
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] index Zero-based set index.
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     */
    void set(int index, bool red, bool green) {
        setDimmed(index, red ? 0xff : 0, green ? 0xff : 0);
    }

    /**
     * @brief Returns or applies the represented bit mask.
     *
     * @param[in] index Zero-based mask index.
     */
    void mask(int index) {
        _mask.set(index);
    }

    /**
     * @brief Clears the requested bits from the mask.
     *
     * @param[in] index Zero-based unmask index.
     */
    void unmask(int index) {
        _mask.reset(index);
    }

    /**
     * @brief Returns the array.
     *
     * @return Reference to the array.
     */
    const LedArray &array() const { return _array; }

private:
    LedArray _array; ///< Owned fixed-size collection of array.
    /**
     * @brief Pointer to mask; `nullptr` denotes that no object/resource is assigned.
     */
    std::bitset<CONFIG_BLM_ROWS * CONFIG_BLM_COLS_LED> _mask; ///< Pointer to mask; `nullptr` denotes that no object/resource is assigned.
};
