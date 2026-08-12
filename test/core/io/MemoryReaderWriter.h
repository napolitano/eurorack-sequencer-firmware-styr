/**
 * @file MemoryReaderWriter.h
 * @brief Declares the MemoryReaderWriter component used by the unit test.
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
#include <vector>
#include <cstdint>
#include <cstring>
// Test-only dynamic memory writer/reader to avoid fixed-buffer truncation in tests.
class MemoryWriter {
public:
    MemoryWriter(void *buf, size_t len) : _outBuf(reinterpret_cast<uint8_t*>(buf)), _outCap(len) {
        _overflowed = false;
    }
    void write(const void *data, size_t len) {
        if (!data || len == 0) return;
        const uint8_t *p = reinterpret_cast<const uint8_t*>(data);
        try {
            // Append to internal dynamic buffer (keeps full payload for inspection)
            _buf.insert(_buf.end(), p, p + len);
        } catch (...) {
            _overflowed = true;
        }

        // Also copy into provided fixed buffer up to its capacity, if present
        if (_outBuf && _outCap > 0) {
            size_t already = _outPos;
            size_t avail = (_outCap > already) ? (_outCap - already) : 0;
            size_t toCopy = (len <= avail) ? len : avail;
            if (toCopy) {
                std::memcpy(_outBuf + already, p, toCopy);
                _outPos += toCopy;
            }
            if (toCopy < len) {
                // indicate we couldn't write whole payload into the provided buffer
                _overflowed = true;
            }
        }
    }
    size_t bytesWritten() const { return _buf.size(); }
    const std::vector<uint8_t>& buffer() const { return _buf; }
    bool overflowed() const { return _overflowed; }
private:
    std::vector<uint8_t> _buf;
    bool _overflowed;
    uint8_t* _outBuf = nullptr;
    size_t _outCap = 0;
    size_t _outPos = 0;
};
class MemoryReader {
public:
    MemoryReader(const void *buf, size_t len) :
        _buf(reinterpret_cast<const uint8_t *>(buf)),
        _len(len),
        _pos(_buf)
    {}
    void read(void *data, size_t len) {
        uint8_t *out = reinterpret_cast<uint8_t *>(data);
        size_t avail = 0;
        if (_pos <= _buf + _len) avail = size_t((_buf + _len) - _pos);
        size_t toCopy = (len <= avail) ? len : avail;
        if (toCopy) {
            std::memcpy(out, _pos, toCopy);
            _pos += toCopy;
        }
        if (toCopy < len) {
            std::memset(out + toCopy, 0, len - toCopy);
        }
    }
    size_t bytesRead() const { return _pos - _buf; }
private:
    const uint8_t *_buf;
    size_t _len;
    const uint8_t *_pos;
};
