/**
 * @file VersionedSerializedReader.h
 * @brief Declares the VersionedSerializedReader component used by the shared core support.
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
 * @brief Reads versioned serialized data while maintaining compatibility and checksum state.
 */
class VersionedSerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;
    /**
     * @brief Constructs a VersionedSerializedReader instance.
     *
     * @param[in] reader Reader that supplies serialized input data.
     * @param[in] readerVersion Maximum serialization version understood by this reader.
     */
    VersionedSerializedReader(Reader reader, uint32_t readerVersion) :
        /**
         * @brief Returns the reader.
         */
        _reader(reader),
        /**
         * @brief Returns the reader version.
         */
        _readerVersion(readerVersion)
    {
        _reader(&_dataVersion, sizeof(_dataVersion));
    }
    /**
     * @brief Reads er version.
     *
     * @return Serialized-data reader implementation version.
     */
    uint32_t readerVersion() const { return _readerVersion; }
    /**
     * @brief Returns the data version.
     *
     * @return Version encoded in the serialized data stream.
     */
    uint32_t dataVersion() const { return _dataVersion; }
    template<typename T>
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] addedInVersion First data-format version in which the field is present.
     */
    void read(T &value, uint32_t addedInVersion = 0) {
        read(&value, sizeof(value), addedInVersion);
    }
    // This is potentially slow as we call serialize for every enum value until a match is found.
    // However, the nice thing about using a switch state in the serialize function is that the
    // compiler can warn us when new enum values are added without updating the serialize function.
    template<typename Enum, typename SerializeFunc>
    /**
     * @brief Reads enum.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     * @param[in] serialize Serializer/deserializer that carries persistent data.
     * @param[in] addedInVersion First data-format version in which the field is present.
     */
    void readEnum(Enum &e, SerializeFunc serialize, uint32_t addedInVersion = 0) {
        if (_dataVersion >= addedInVersion) {
            auto i = serialize(Enum(0));
            read(i);
            for (e = Enum(0); int(e) < int(Enum::Last); e = Enum(int(e) + 1)) {
                if (serialize(e) == i) {
                    return;
                }
            }
            e = Enum(0);
        }
    }
    template<typename ReadT, typename T>
    /**
     * @brief Reads as.
     *
     * @param[out] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] addedInVersion First data-format version in which the field is present.
     */
    void readAs(T &value, uint32_t addedInVersion = 0) {
        if (_dataVersion >= addedInVersion) {
            ReadT tmp;
            read(tmp);
            value = tmp;
        }
    }
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     * @param[in] addedInVersion First data-format version in which the field is present.
     */
    void read(void *data, size_t len, uint32_t addedInVersion) {
        if (_dataVersion >= addedInVersion) {
            _reader(data, len);
            _hash(data, len);
        }
    }
    template<typename T>
    /**
     * @brief Advances the reader without decoding the skipped field/value.
     *
     * @param[in] addedInVersion First data-format version in which the field is present.
     * @param[in] removedInVersion First data-format version in which the field is absent.
     */
    void skip(uint32_t addedInVersion, uint32_t removedInVersion) {
        skip(sizeof(T), addedInVersion, removedInVersion);
    }
    /**
     * @brief Advances the reader without decoding the skipped field/value.
     *
     * @param[in] len Number of valid bytes, characters, or elements.
     * @param[in] addedInVersion First data-format version in which the field is present.
     * @param[in] removedInVersion First data-format version in which the field is absent.
     */
    void skip(size_t len, uint32_t addedInVersion, uint32_t removedInVersion) {
        if (_dataVersion >= addedInVersion && _dataVersion < removedInVersion) {
            uint8_t dummy[len];
            _reader(dummy, len);
            _hash(dummy, len);
        }
    }
    /**
     * @brief Returns the check hash.
     *
     * @return `true` if hash; otherwise `false`.
     */
    bool checkHash() {
        uint32_t hash;
        _reader(&hash, sizeof(hash));
        return _hash.result() == hash;
    }
    /**
     * @brief Saves the current rolling serialization hash state.
     */
    void backupHash() {
        _savedHash = _hash;
    }
    /**
     * @brief Restores the previously saved rolling serialization hash state.
     */
    void restoreHash() {
        _hash = _savedHash;
    }
private:
    Reader _reader; ///< Underlying byte reader supplying the versioned serialization stream.
    /**
     * @brief Runtime value representing reader version.
     */
    uint32_t _readerVersion; ///< Serialization schema version supported by the reader.
    /**
     * @brief Runtime value representing data version.
     */
    uint32_t _dataVersion; ///< Version encoded in the serialized data being read.
    /**
     * @brief Current rolling hash value.
     */
    FnvHash _hash; ///< Current rolling hash value.
    FnvHash _savedHash; ///< Saved rolling hash used while temporarily excluding fields from checksum validation.
};
