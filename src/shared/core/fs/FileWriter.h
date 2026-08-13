/**
 * @file FileWriter.h
 * @brief Declares the FileWriter component used by the shared filesystem support.
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

#include "File.h"

#include <algorithm>

#include <cstring>
#include <cstddef>
#include <cstdint>

namespace fs {

/**
 * File writer.
 * Buffers writes to increase throughput and keeps track of potential errors, which are returned when calling finish().
 */
class FileWriter {
public:
    /**
     * @brief Constructs a FileWriter instance.
     *
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     */
    FileWriter(const char *path) {
        _error = _file.open(path, File::Write);
    }

    /**
     * @brief Destroys the FileWriter instance.
     */
    ~FileWriter() {
        finish();
    }

    /**
     * @brief Returns the error.
     *
     * @return Most recent filesystem/stream error status.
     */
    Error error() const { return _error; }

    /**
     * @brief Finalizes the current operation and commits pending result state.
     *
     * @return Final serialization/file status after completing the operation.
     */
    Error finish() {
        if (!_finished) {
            if (_error == OK) {
                _error = _file.writeAll(_buffer, _pos);
            }
            if (_error == OK) {
                _error = _file.close();
            } else {
                _file.close();
            }
            _finished = true;
        }
        return _error;
    }

    /**
     * @brief Writes data to the underlying destination.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] len Number of valid bytes, characters, or elements.
     *
     * @return Result of write().
     */
    Error write(const void *data, size_t len) {
        const uint8_t *src = static_cast<const uint8_t *>(data);
        uint8_t *buffer = reinterpret_cast<uint8_t *>(_buffer);
        while (_error == OK && len > 0) {
            size_t chunk = std::min(len, BufferSize - _pos);
            memcpy(&buffer[_pos], src, chunk);
            _pos += chunk;
            src += chunk;
            len -= chunk;
            if (_pos == BufferSize) {
                _pos = 0;
                _error = _file.writeAll(buffer, BufferSize);
            }
        }
        return _error;
    }

private:
    /**
     * @brief Buffer size constant used by this component.
     */
    static constexpr size_t BufferSize = 512; ///< Buffered filesystem I/O block size, in bytes.

    File _file; ///< File handle to which serialized/output bytes are written.
    /**
     * @brief Whether finished is true in the current state.
     */
    bool _finished = false; ///< True after the operation has reached its terminal state and no additional payload remains to be processed.
    Error _error; ///< Current/last error status.
    /**
     * @brief Backing buffer used to stage data for the surrounding operation.
     */
    uint32_t _buffer[BufferSize / 4]; ///< Backing buffer used to stage data for the surrounding operation.
    /**
     * @brief Current read/write position within the active buffer/stream.
     */
    size_t _pos = 0; ///< Current read/write position within the active buffer/stream.
};

} // namespace fs
