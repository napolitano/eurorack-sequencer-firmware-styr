/**
 * @file Container.h
 * @brief Declares the Container component used by the shared core support.
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
#include <cstdint>
#include <new>

template<typename... Ts>
struct maxsizeof {
    static constexpr size_t value = 0;
};

template<typename T, typename... Ts>
struct maxsizeof<T, Ts...> {
    static constexpr size_t value = sizeof(T) > maxsizeof<Ts...>::value ? sizeof(T) : maxsizeof<Ts...>::value;
};

template<typename... Ts>
class Container {
public:
    static constexpr size_t Size = maxsizeof<Ts...>::value;

    template<typename U, typename... Args>
    U *create(Args&&... args) {
        return new(_data) U(args...);
    }

    template<typename U>
    void destroy(U *object) {
        if (object) {
            object->~U();
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    template<typename U>
    U &as() {
        return *reinterpret_cast<U *>(_data);
    }

    template<typename U>
    const U &as() const {
        return *reinterpret_cast<const U *>(_data);
    }

#pragma GCC diagnostic pop

private:
    // memory aligned to system pointer size
    uintptr_t _data[(Size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)];
};
