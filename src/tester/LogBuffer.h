/**
 * @file LogBuffer.h
 * @brief Declares the LogBuffer component used by the hardware tester firmware.
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

#include <algorithm>

#include <cstring>

/**
 * @brief Stores a fixed number of recent text lines in chronological ring-buffer order.
 */
template<size_t Lines, size_t LineLength>
/**
 * @brief Stores a bounded rolling text log for tester/debug output.
 */
class LogBuffer {
public:
    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _index = 0;
        _lines = 0;
    }

    /**
     * @brief Writes the object representation to the supplied output.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     */
    void print(const char *text) {
        strncpy(_buffer[_index], text, LineLength);
        _buffer[_index][LineLength - 1] = '\0';

        _index = (_index + 1) % Lines;
        _lines = std::min(_lines + 1, Lines);
    }

    /**
     * @brief Returns the lines.
     *
     * @return Number of valid text lines currently stored.
     */
    size_t lines() const {
        return _lines;
    };

    /**
     * @brief Returns a stored log line in chronological order.
     *
     * @param[in] index Zero-based line index.
     *
     * @return Null-terminated text for the requested retained line.
     */
    const char *line(int index) const {
        index = (index + _index + Lines - _lines) % Lines;
        return _buffer[index];
    }
private:
    /**
     * @brief Zero-based index; a negative/sentinel value represents no selection where applicable.
     */
    size_t _index = 0; ///< Ring-buffer slot that will receive the next printed line.
    /**
     * @brief Runtime value representing lines.
     */
    size_t _lines = 0; ///< Number of text lines currently stored in the log buffer.
    char _buffer[Lines][LineLength]; ///< Fixed storage for `Lines` null-terminated log strings, each limited to `LineLength - 1` characters. ///< Fixed storage for `Lines` null-terminated log strings, each limited to `LineLength - 1` characters.
};
