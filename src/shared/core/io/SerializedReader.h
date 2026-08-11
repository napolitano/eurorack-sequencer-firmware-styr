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

class SerializedReader {
public:
    typedef std::function<void(void *, size_t)> Reader;

    SerializedReader(Reader reader) :
        _reader(reader)
    {}

    template<typename T>
    void read(T &value) {
        read(&value, sizeof(value));
    }

    template<typename ReadT, typename T>
    void readAs(T &value) {
        ReadT tmp;
        read(tmp);
        value = tmp;
    }

    void read(void *data, size_t len) {
        _reader(data, len);
    }

private:
    Reader _reader;
};
