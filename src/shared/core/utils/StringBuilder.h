/**
 * @file StringBuilder.h
 * @brief Declares the StringBuilder component used by the shared core support.
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

#include "stb/stb_sprintf.h"

#include <cstdarg>
#include <cstdlib>

/**
 * @brief Appends formatted text into a caller-owned bounded character buffer.
 */
class StringBuilder {
public:
    /**
     * @brief Constructs a StringBuilder instance.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    StringBuilder(char *buf, size_t len) :
        /**
         * @brief Returns the buf.
         */
        _buf(buf),
        /**
         * @brief Returns the len.
         */
        _len(len)
    {
        reset();
    }

    /**
     * @brief Resets the StringBuilder to its initial runtime state.
     */
    void reset() {
        _pos = _buf;
        _buf[0] = 0;
    }

    /**
     * @brief Formats the f into the supplied string builder/output.
     *
     * @param[in] fmt Printf-style format string.
     * @param[in] args Argument collection forwarded to the operation.
     *
     * @return Reference to the printf.
     */
    StringBuilder &printf(const char *fmt, ...) {
        if (fmt) {
            va_list va;
            va_start(va, fmt);
            _pos += stbsp_vsnprintf(_pos, _len - (_pos - _buf), fmt, va);
            va_end(va);
        }
        return *this;
    }

    template<typename ...Args>
    /**
     * @brief Implements operator() for this type.
     *
     * @param[in] fmt Printf-style format string.
     * @param[in] args Argument collection forwarded to the operation.
     *
     * @return Reference to the operator().
     */
    StringBuilder &operator()(char const *fmt, Args... args) {
        return printf(fmt, args...);
    }

    operator const char *() const { return _buf; }

private:
    /**
     * @brief Pointer to buf; `nullptr` denotes that no object/resource is assigned.
     */
    char *_buf; ///< Pointer to buf; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Runtime value representing len.
     */
    size_t _len; ///< Number of characters currently stored in the string buffer.
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    char *_pos; ///< Current read/write position within the active buffer/stream.
};

/**
 * Class to generate strings with fixed buffer on stack.
 * example: FixedStringBuilder<32>("item %d", 1)(" ")("item %d", 2)
 */
template<size_t Length>
/**
 * @brief Formats text into a caller-owned fixed-capacity character buffer without dynamic allocation.
 */
class FixedStringBuilder : public StringBuilder {
public:
    /**
     * @brief Constructs a FixedStringBuilder instance.
     */
    FixedStringBuilder() :
        /**
         * @brief Returns the string builder.
         */
        StringBuilder(_buf, Length)
    {}

    template<typename ...Args>
    /**
     * @brief Constructs a FixedStringBuilder instance.
     *
     * @param[in] fmt Printf-style format string.
     * @param[in] args Argument collection forwarded to the operation.
     */
    FixedStringBuilder(const char *fmt, Args... args) :
        /**
         * @brief Returns the string builder.
         */
        StringBuilder(_buf, Length)
    {
        printf(fmt, args...);
    }

private:
    /**
     * @brief Runtime value representing buf.
     */
    char _buf[Length]; ///< Owned fixed-size collection of buf.
};
