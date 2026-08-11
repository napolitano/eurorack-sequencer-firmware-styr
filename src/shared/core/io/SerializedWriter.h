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

class SerializedWriter {
public:
    typedef std::function<void(const void *, size_t)> Writer;

    SerializedWriter(Writer writer) :
        _writer(writer)
    {}

    template<typename T>
    void write(const T &value) {
        write(&value, sizeof(value));
    }

    void write(const void *data, size_t len) {
        _writer(data, len);
    }

private:
    Writer _writer;
};
