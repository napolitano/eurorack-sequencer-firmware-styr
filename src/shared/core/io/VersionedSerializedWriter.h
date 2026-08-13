/**
 * @file VersionedSerializedWriter.h
 * @brief Declares the VersionedSerializedWriter component used by the shared core support.
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
#include "core/hash/FnvHash.h"
#include <cstdlib>
#include <cstdint>
#include <functional>
/**
 * @brief Writes versioned serialized data together with version and checksum metadata.
 */
class VersionedSerializedWriter {
public:
    typedef std::function<void(const void *, size_t)> Writer;
    /**
     * @brief Constructs a VersionedSerializedWriter instance.
     *
     * @param[in] writer Writer that receives serialized output data.
     * @param[in] writerVersion Serialization version emitted by this writer.
     */
    VersionedSerializedWriter(Writer writer, uint32_t writerVersion) :
        /**
         * @brief Returns the writer.
         */
        _writer(writer),
        /**
         * @brief Returns the writer version.
         */
        _writerVersion(writerVersion)
    {
        _writer(&_writerVersion, sizeof(_writerVersion));
    }
    /**
     * @brief Writes r version.
     *
     * @return Serialization version emitted by this writer.
     */
    uint32_t writerVersion() const { return _writerVersion; }
    template<typename T>
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] value Value to write.
     */
    void write(const T &value) {
        write(&value, sizeof(value));
    }
    template<typename Enum, typename SerializeFunc>
    /**
     * @brief Writes enum.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     * @param[in] serialize Serializer/deserializer that carries persistent data.
     */
    void writeEnum(Enum e, SerializeFunc serialize) {
        auto value = serialize(e);
        write(value);
    }
    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void write(const void *data, size_t len) {
        _hash(data, len);
        _writer(data, len);
    }
    /**
     * @brief Writes hash.
     */
    void writeHash() {
        uint32_t hash = _hash.result();
        _writer(&hash, sizeof(hash));
    }
private:
    Writer _writer; ///< Underlying byte writer receiving the versioned serialization stream.
    /**
     * @brief Runtime value representing writer version.
     */
    uint32_t _writerVersion; ///< Serialization schema version emitted by the writer.
    /**
     * @brief Current rolling hash value.
     */
    FnvHash _hash; ///< Current rolling hash value.
};
