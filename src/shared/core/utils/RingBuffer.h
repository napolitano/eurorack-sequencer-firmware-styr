/**
 * @file RingBuffer.h
 * @brief Declares the RingBuffer component used by the shared core support.
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

#include <cstddef>

/**
 * @brief Provides fixed-capacity FIFO storage with nonblocking read/write helpers.
 */
template<typename T, size_t Size>
/**
 * @brief Implements a fixed-capacity FIFO ring buffer without dynamic allocation.
 */
class RingBuffer {
public:
    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    inline size_t size() const { return Size; }

    /**
     * @brief Returns the empty.
     *
     * @return `true` if empty; otherwise `false`.
     */
    inline bool empty() const { return _read == _write; }

    /**
     * @brief Returns the full.
     *
     * @return `true` if full; otherwise `false`.
     */
    inline bool full() const { return writable() == 0; }

    /**
     * @brief Returns the entries.
     *
     * @return Number of elements currently stored in the buffer.
     */
    inline size_t entries() const { return (_write - _read) % Size; }

    /**
     * @brief Returns the writable.
     *
     * @return Number of additional elements that can be written without overflow.
     */
    inline size_t writable() const {
        return (_read - _write - 1) % Size;
    }

    /**
     * @brief Reads able.
     *
     * @return Number of elements currently available to read.
     */
    inline size_t readable() const {
        return (_write - _read) % Size;
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] value Value to write.
     */
    inline void write(T value) {
        size_t write = _write;
        _buffer[write] = value;
        _write = (write + 1) % Size;
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    inline void write(const T *data, size_t length) {
        while (length--) {
            write(*data++);
        }
    }

    /**
     * @brief Returns the read.
     *
     * @return `true` when an element was read; `false` when the buffer was empty.
     */
    inline T read() {
        size_t read = _read;
        T value = _buffer[read];
        _read = (read + 1) % Size;
        return value;
    }

    /**
     * @brief Reads and replace.
     *
     * @param[in] replacement Replacement element written when the ring-buffer operation succeeds.
     *
     * @return Result of readAndReplace().
     */
    inline T readAndReplace(const T &replacement = T()) {
        size_t read = _read;
        T value = _buffer[read];
        _buffer[read] = replacement;
        _read = (read + 1) % Size;
        return value;
    }

    /**
     * @brief Reads a value/data from the source.
     *
     * @param[out] data Input data to read, decode, copy, or process.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    inline void read(T *data, size_t length) {
        while (length--) {
            read(*data++);
        }
    }

private:
    /**
     * @brief Backing buffer used to stage data for the surrounding operation.
     */
    T _buffer[Size]; ///< Backing buffer used to stage data for the surrounding operation.
    /**
     * @brief Runtime value representing read.
     */
    volatile size_t _read = 0; ///< Read cursor/index used by the ring buffer.
    /**
     * @brief Runtime value representing write.
     */
    volatile size_t _write = 0; ///< Write cursor/index used by the ring buffer.
};
