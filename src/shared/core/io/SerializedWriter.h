/**
 * @file SerializedWriter.h
 * @brief Declares the SerializedWriter component used by the shared core support.
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

#include <cstdint>
#include <functional>

/**
 * @brief Writes typed values to a byte-oriented output writer.
 */
class SerializedWriter {
public:
    typedef std::function<void(const void *, size_t)> Writer;

    /**
     * @brief Constructs a SerializedWriter instance.
     *
     * @param[in] writer Writer that receives serialized output data.
     */
    SerializedWriter(Writer writer) :
        /**
         * @brief Returns the writer.
         */
        _writer(writer)
    {}

    template<typename T>
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] value Value to write.
     */
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void write(const void *data, size_t len) {
        _writer(data, len);
    }

private:
    Writer _writer; ///< Underlying byte writer receiving serialized output.
};
