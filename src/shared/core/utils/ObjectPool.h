/**
 * @file ObjectPool.h
 * @brief Declares the ObjectPool component used by the shared core support.
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

#include <array>

#include <cstdlib>

/**
 * @brief Allocates objects from a fixed-capacity pool without heap allocation.
 */
template<typename T, size_t Capacity>
/**
 * @brief Provides fixed-capacity object allocation without dynamic memory.
 */
class ObjectPool {
public:
    /**
     * @brief Returns the capacity.
     *
     * @return Maximum number of elements the container can hold.
     */
    size_t capacity() const { return Capacity; }
    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() { return _size; }

    /**
     * @brief Returns the allocate.
     *
     * @return Pointer to the allocate; `nullptr` when no value is available.
     */
    T *allocate() {
        for (auto &item : _items) {
            if (!item.used) {
                item.used = true;
                --_size;
                return &item.object;
            }
        }
        return nullptr;
    }

    /**
     * @brief Releases the supplied resource or object.
     *
     * @param[in] object Object read, stored, or modified by the operation.
     */
    void release(T *object) {
        if (object) {
            Item *item = reinterpret_cast<Item *>(object);
            item->used = false;
            ++_size;
        }
    }

    template<typename Func>
    /**
     * @brief Invokes the supplied callback for every stored item.
     *
     * @param[in] func Function/callback invoked by the operation.
     */
    void forEach(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                func(item.object);
            }
        }
    }

private:
    /**
     * @brief Stores the fields required to represent item.
     */
    struct Item {
        T object; ///< In-place storage occupied by the pooled object while this slot is allocated.
        /**
         * @brief Whether used is true in the current state.
         */
        bool used = 0; ///< True when this pool/file slot currently contains a live entry.
    };

    /**
     * @brief Fixed-capacity pool slots storing constructed objects and allocation state.
     */
    std::array<Item, Capacity> _items; ///< Fixed-capacity pool slots storing constructed objects and allocation state.
    /**
     * @brief Size of the associated data in bytes/elements as defined by this type.
     */
    size_t _size = Capacity; ///< Size of the associated data in bytes/elements as defined by this type.
};
