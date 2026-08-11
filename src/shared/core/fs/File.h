/**
 * @file File.h
 * @brief Declares the File component used by the shared filesystem support.
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

#include "Error.h"

#include "ff/ff.h"

#include <cstddef>
#include <cstdint>

namespace fs {

class File {
public:
    enum Mode {
        Read,
        Write,
        Append,
    };

    File() = default;

    File(const char *path, Mode mode) {
        open(path, mode);
    }

    ~File() {
        close();
    }

    Error open(const char *path, Mode mode) {
        _file = allocateFile();

        switch (mode) {
        case Read:      _error = Error(f_open(_file, path, FA_READ)); break;
        case Write:     _error = Error(f_open(_file, path, FA_WRITE | FA_CREATE_ALWAYS)); break;
        case Append:    _error = Error(f_open(_file, path, FA_WRITE | FA_OPEN_APPEND)); break;
        default:        _error = INVALID_PARAMETER;
        }
        return _error;
    }

    Error close() {
        if (_file) {
            _error = Error(f_close(_file));
            releaseFile(_file);
            _file = nullptr;
            return _error;
        }
        return OK;
    }

    Error write(const void *buf, size_t len, size_t *lenWritten = nullptr) {
        UINT bw;
        _error = Error(f_write(_file, buf, len, &bw));
        if (lenWritten) {
            *lenWritten = bw;
        }
        return _error;
    }

    Error writeAll(const void *buf, size_t len) {
        size_t lenWritten;
        write(buf, len, &lenWritten);
        if (_error == OK && len != lenWritten) {
            _error = DISK_FULL;
        }
        return _error;
    }

    Error read(void *buf, size_t len, size_t *lenRead = nullptr) {
        UINT br;
        _error = Error(f_read(_file, buf, len, &br));
        if (lenRead) {
            *lenRead = br;
        }
        return _error;
    }

    bool eof() {
        return f_eof(_file);
    }

    size_t size() const {
        return f_size(_file);
    }

    size_t tell() const {
        return f_tell(_file);
    }

    Error seek(size_t offset) {
        _error = Error(f_lseek(_file, offset));
        return _error;
    }

    Error truncate() {
        _error = Error(f_truncate(_file));
        return _error;
    }

    Error sync() {
        _error = Error(f_sync(_file));
        return _error;
    }

    Error error() const {
        return _error;
    }

private:
    static FIL *allocateFile();
    static void releaseFile(FIL *file);

    FIL *_file = nullptr;
    Error _error = OK;
};

} // namespace fs
