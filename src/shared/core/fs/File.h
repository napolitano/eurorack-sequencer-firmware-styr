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

/**
 * @brief Wraps a FatFs file handle and provides positioned byte I/O.
 */
class File {
public:
    /**
     * @brief Enumerates the supported mode values.
     */
    enum Mode {
        Read, ///< Selects the read mode.
        Write, ///< Selects the write mode.
        Append, ///< Selects the append mode.
    };

    /**
     * @brief Constructs a File instance.
     */
    File() = default;

    /**
     * @brief Constructs a File instance.
     *
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     * @param[in] mode Mode to select.
     */
    File(const char *path, Mode mode) {
        open(path, mode);
    }

    /**
     * @brief Destroys the File instance.
     */
    ~File() {
        close();
    }

    /**
     * @brief Opens the requested page, file, dialog, or resource.
     *
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     * @param[in] mode Mode to select.
     *
     * @return Result of open().
     */
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

    /**
     * @brief Closes the active page, file, dialog, or resource.
     *
     * @return Filesystem status returned by the close operation.
     */
    Error close() {
        if (_file) {
            _error = Error(f_close(_file));
            releaseFile(_file);
            _file = nullptr;
            return _error;
        }
        return OK;
    }

    /**
     * @brief Writes data to the underlying destination.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] len Number of valid bytes, characters, or elements.
     * @param[in] lenWritten Destination receiving the number of bytes successfully written.
     *
     * @return Result of write().
     */
    Error write(const void *buf, size_t len, size_t *lenWritten = nullptr) {
        UINT bw;
        _error = Error(f_write(_file, buf, len, &bw));
        if (lenWritten) {
            *lenWritten = bw;
        }
        return _error;
    }

    /**
     * @brief Writes all.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] len Number of valid bytes, characters, or elements.
     *
     * @return Result of writeAll().
     */
    Error writeAll(const void *buf, size_t len) {
        size_t lenWritten;
        write(buf, len, &lenWritten);
        if (_error == OK && len != lenWritten) {
            _error = DISK_FULL;
        }
        return _error;
    }

    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] len Number of valid bytes, characters, or elements.
     * @param[in] lenRead Destination receiving the number of bytes successfully read.
     *
     * @return Result of read().
     */
    Error read(void *buf, size_t len, size_t *lenRead = nullptr) {
        UINT br;
        _error = Error(f_read(_file, buf, len, &br));
        if (lenRead) {
            *lenRead = br;
        }
        return _error;
    }

    /**
     * @brief Returns the eof.
     *
     * @return `true` if eof; otherwise `false`.
     */
    bool eof() {
        return f_eof(_file);
    }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const {
        return f_size(_file);
    }

    /**
     * @brief Returns the tell.
     *
     * @return Current byte offset of the file cursor.
     */
    size_t tell() const {
        return f_tell(_file);
    }

    /**
     * @brief Returns seek.
     *
     * @param[in] offset Offset applied in the domain defined by the operation.
     *
     * @return Result of seek().
     */
    Error seek(size_t offset) {
        _error = Error(f_lseek(_file, offset));
        return _error;
    }

    /**
     * @brief Returns the truncate.
     *
     * @return Filesystem status returned by the truncate operation.
     */
    Error truncate() {
        _error = Error(f_truncate(_file));
        return _error;
    }

    /**
     * @brief Synchronizes the File with its current source of truth.
     *
     * @return Filesystem status returned after flushing file data.
     */
    Error sync() {
        _error = Error(f_sync(_file));
        return _error;
    }

    /**
     * @brief Returns the error.
     *
     * @return Most recent filesystem/stream error status.
     */
    Error error() const {
        return _error;
    }

private:
    /**
     * @brief Returns the allocate file.
     *
     * @return Pointer to the allocate file; `nullptr` when no value is available.
     */
    static FIL *allocateFile();
    /**
     * @brief Releases the active file handle.
     *
     * @param[in] file File object whose current position/content is read or updated.
     */
    static void releaseFile(FIL *file);

    /**
     * @brief Pointer to file; `nullptr` denotes that no object/resource is assigned.
     */
    FIL *_file = nullptr; ///< Pointer to file; `nullptr` denotes that no object/resource is assigned.
    Error _error = OK; ///< Current/last error status.
};

} // namespace fs
