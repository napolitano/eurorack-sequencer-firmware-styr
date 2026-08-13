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

/**
 * @brief Stores the fields required to represent maxsizeof.
 */
template<typename... Ts>
struct maxsizeof {
    /**
     * @brief Value constant used by this component.
     */
    static constexpr size_t value = 0; ///< Compile-time maximum size of the remaining type pack; zero for the empty pack.
};

/**
 * @brief Stores the fields required to represent maxsizeof.
 */
template<typename T, typename... Ts>
struct maxsizeof<T, Ts...> {
    /**
     * @brief Returns the sizeof.
     *
     * @return The sizeof value.
     */
    static constexpr size_t value = sizeof(T) > maxsizeof<Ts...>::value ? sizeof(T) : maxsizeof<Ts...>::value; ///< Compile-time maximum `sizeof` value across this type and the remaining type pack. ///< Compile-time maximum `sizeof` value across this type and the remaining type pack.
};

/**
 * @brief Provides fixed in-place storage large enough for any one of the configured types.
 */
template<typename... Ts>
/**
 * @brief Provides fixed inline storage for one object selected from a compile-time type list.
 */
class Container {
public:
    /**
     * @brief Size constant used by this component.
     */
    static constexpr size_t Size = maxsizeof<Ts...>::value; ///< Number of bytes reserved so the container can hold the largest supported type.

    template<typename U, typename... Args>
    /**
     * @brief Constructs an object of type `U` in the container's internal storage.
     *
     * @param[in] args Argument collection forwarded to the operation.
     *
     * @return Pointer to the newly placement-constructed object in the internal storage.
     */
    U *create(Args&&... args) {
        return new(_data) U(args...);
    }

    template<typename U>
    /**
     * @brief Destroys the currently stored object/resource.
     *
     * @param[in] object Object read, stored, or modified by the operation.
     */
    void destroy(U *object) {
        if (object) {
            object->~U();
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    template<typename U>
    /**
     * @brief Returns the as.
     *
     * @return Reference to the as.
     */
    U &as() {
        return *reinterpret_cast<U *>(_data);
    }

    template<typename U>
    /**
     * @brief Returns the as.
     *
     * @return Reference to the as.
     */
    const U &as() const {
        return *reinterpret_cast<const U *>(_data);
    }

#pragma GCC diagnostic pop

private:
    // memory aligned to system pointer size
    uintptr_t _data[(Size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)];
};
