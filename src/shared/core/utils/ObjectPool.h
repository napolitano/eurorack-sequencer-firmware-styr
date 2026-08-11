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

template<typename T, size_t Capacity>
class ObjectPool {
public:
    size_t capacity() const { return Capacity; }
    size_t size() { return _size; }

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

    void release(T *object) {
        if (object) {
            Item *item = reinterpret_cast<Item *>(object);
            item->used = false;
            ++_size;
        }
    }

    template<typename Func>
    void forEach(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                func(item.object);
            }
        }
    }

private:
    struct Item {
        T object;
        bool used = 0;
    };

    std::array<Item, Capacity> _items;
    size_t _size = Capacity;
};
