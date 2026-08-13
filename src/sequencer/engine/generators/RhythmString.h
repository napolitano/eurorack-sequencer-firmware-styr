/**
 * @file RhythmString.h
 * @brief Declares the RhythmString component used by the sequencer generator.
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

#include <bitset>

/**
 * @brief Parses a textual rhythm pattern into a fixed set of active/inactive steps.
 */
template<size_t N>
/**
 * @brief Stores and edits a fixed-capacity binary rhythm pattern.
 */
class RhythmString {
public:
    /**
     * @brief Constructs a RhythmString instance.
     */
    RhythmString() = default;
    /**
     * @brief Constructs a RhythmString instance.
     *
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    RhythmString(size_t size) : _size(size) {}

    /**
     * @brief Returns the capacity.
     *
     * @return Maximum number of elements the container can hold.
     */
    size_t capacity() const { return N; }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const { return _size; }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _size = 0;
        _steps.reset();
    }

    /**
     * @brief Changes the logical or storage size of the object.
     *
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void resize(size_t size) {
        _size = size;
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] index Zero-based set index.
     * @param[in] value Logical/state value to store or drive.
     */
    void set(size_t index, bool value = true) {
        _size = std::max(_size, index + 1);
        _steps.set(index, value);
    }

    /**
     * @brief Shifts ed.
     *
     * @param[in] offset Offset applied in the domain defined by the operation.
     *
     * @return Value shifted by the requested amount.
     */
    RhythmString<N> shifted(size_t offset) const {
        RhythmString<N> shifted(_size);
        for (size_t i = 0; i < _size; ++i) {
            if (_steps[i]) {
                shifted.set((i + offset) % _size);
            }
        }
        return shifted;
    }

    /**
     * @brief Appends the supplied data/item.
     *
     * @param[in] other Other object/value used for comparison or combination.
     */
    void append(const RhythmString<N> &other) {
        for (size_t i = 0; i < other.size(); ++i) {
            _steps[_size + i] = other[i];
        }
        _size += other.size();
    }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] index Zero-based component/element index.
     *
     * @return `true` if operator[]; otherwise `false`.
     */
    bool operator[](size_t index) const {
        return _steps[index];
    }

private:
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    size_t _size = 0; ///< Size of the associated data in bytes/elements as defined by this type.
    std::bitset<N> _steps; ///< Parsed on/off rhythm steps represented by the source rhythm string.
};
