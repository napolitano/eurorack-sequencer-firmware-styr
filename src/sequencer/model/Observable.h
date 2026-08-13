/**
 * @file Observable.h
 * @brief Declares the Observable component used by the sequencer model.
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

#include "core/Debug.h"

#include <array>
#include <functional>

#include <cstdint>
#include <cstddef>

/**
 * @brief Stores and manipulates observable model data.
 */
template<typename Event, size_t MaxObservers>
/**
 * @brief Stores and manipulates observable model data.
 */
class Observable {
public:
    typedef std::function<void(Event event)> Handler;

    /**
     * @brief Registers or evaluates the watched value.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void watch(Handler handler) {
        ASSERT(_observerCount < MaxObservers, "too many observers");
        auto &observer = _observers[_observerCount++];
        observer.handler = handler;
    }

    /**
     * @brief Notifies registered observers about a state change.
     *
     * @param[in] event Event to process.
     */
    void notify(Event event) {
        for (size_t i = 0; i < _observerCount; ++i) {
            _observers[i].handler(event);
        }
    }

private:
    /**
     * @brief Stores and manipulates observer model data.
     */
    struct Observer {
        Handler handler; ///< Callback handler invoked by this component.
    };

    /**
     * @brief Fixed-capacity storage for observers.
     */
    std::array<Observer, MaxObservers> _observers; ///< Fixed observer slots used to store registered callbacks without dynamic allocation.
    /**
     * @brief Number of observer items currently tracked or supported.
     */
    size_t _observerCount = 0; ///< Number of observer items currently tracked or supported.
};
