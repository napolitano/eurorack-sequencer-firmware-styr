/**
 * @file FileDefs.h
 * @brief Declares the FileDefs component used by the sequencer model.
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
#include <cstdint>

/**
 * @brief Enumerates the supported file type values.
 */
enum class FileType : uint8_t {
    Project     = 0, ///< Selects the project file type.
    UserScale   = 1, ///< Selects the user scale file type.
    Settings    = 255 ///< Selects the settings file type.
};

/**
 * @brief Stores and manipulates file header model data.
 */
struct FileHeader {
    /**
     * @brief Name length constant used by this component.
     */
    static constexpr size_t NameLength = 8; ///< Width of the serialized file-header name field, in bytes.

    FileType type; ///< File type associated with the storage operation.
    uint8_t version; ///< Serialized format version stored in this file header.
    char name[NameLength]; ///< Fixed-width serialized name bytes; all `NameLength` bytes may be occupied, so in-place termination is not guaranteed.

    /**
     * @brief Constructs a FileHeader instance.
     */
    FileHeader() = default;

    /**
     * @brief Constructs a FileHeader instance.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] version Firmware or serialized-format version associated with the operation.
     * @param[in] name Name or display string associated with the object.
     */
    FileHeader(FileType type, uint8_t version, const char *name) {
        this->type = type;
        this->version = version;
        size_t len = strlen(name);
        std::memset(this->name, 0, sizeof(this->name));
        std::memcpy(this->name, name, std::min(sizeof(this->name), len));
    }

    /**
     * @brief Reads name.
     *
     * @param[in] name Name or display string associated with the object.
     * @param[in] len Number of valid bytes, characters, or elements.
     */
    void readName(char *name, size_t len) {
        std::memcpy(name, this->name, std::min(sizeof(this->name), len));
        name[std::min(sizeof(this->name), len - 1)] = '\0';
    }

/**
 * @brief Returns attribute.
 *
 * @note Includes an unnamed `(packed)` input parameter as declared by the inherited/interface signature.
 *
 * @return Decoded/configured attribute value.
 */
} __attribute__((packed));

