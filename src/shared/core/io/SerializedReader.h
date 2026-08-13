/**
 * @file SerializedReader.h
 * @brief Declares the SerializedReader component used by the shared core support.
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
 * @brief Reads typed values from a byte-oriented input reader.
 */
class SerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;

    /**
     * @brief Constructs a SerializedReader instance.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    SerializedReader(Reader reader) :
        /**
         * @brief Returns the reader.
         */
        _reader(reader)
    {}

    template<typename T>
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void read(T &value) {
        read(&value, sizeof(value));
    }

    template<typename ReadT, typename T>
    /**
     * @brief Reads as.
     *
     * @param[out] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void readAs(T &value) {
        ReadT tmp;
        read(tmp);
        value = tmp;
    }

    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void read(void *data, size_t len) {
        _reader(data, len);
    }

private:
    Reader _reader; ///< Underlying byte reader supplying serialized input.
};
