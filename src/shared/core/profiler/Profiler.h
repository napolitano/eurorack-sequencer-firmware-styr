/**
 * @file Profiler.h
 * @brief Declares the Profiler component used by the shared core support.
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

#include "SystemConfig.h"

#include "drivers/HighResolutionTimer.h"

#include <cstdint>

#if CONFIG_ENABLE_PROFILER

/**
 * @brief Implements profiler operations and maintains the associated runtime state.
 */
class Profiler {
public:
    /**
     * @brief Initializes the Profiler and its runtime resources.
     */
    static void init();
    /**
     * @brief Writes diagnostic state to the configured debug output.
     */
    static void dump();

    /**
     * @brief Stores the fields required to represent interval.
     */
    struct Interval {
        /**
         * @brief Constructs a Interval instance.
         *
         * @param[in] desc Descriptor containing metadata for the registered object or profiler item.
         */
        Interval(const char *desc) : desc(desc) {
            registerInterval(this);
        }

        /**
         * @brief Begins the operation or measurement interval.
         */
        inline void begin() {
            start = HighResolutionTimer::us();
        }

        /**
         * @brief Ends the operation or measurement interval.
         */
        inline void end() {
            uint32_t end = HighResolutionTimer::us();
            avg = end - start;
        }

        /**
         * @brief Pointer to desc; `nullptr` denotes that no object/resource is assigned.
         */
        const char *desc; ///< Pointer to desc; `nullptr` denotes that no object/resource is assigned.
        /**
         * @brief Runtime value representing start.
         */
        uint32_t start; ///< Timestamp or starting value captured when the interval begins.
        /**
         * @brief Runtime value representing avg.
         */
        uint32_t avg; ///< Running average recorded for this profiler/statistics entry.
    };

    /**
     * @brief Stores the fields required to represent counter.
     */
    struct Counter {
        /**
         * @brief Constructs a Counter instance.
         *
         * @param[in] desc Descriptor containing metadata for the registered object or profiler item.
         */
        Counter(const char *desc) : desc(desc) {
            registerCounter(this);
        }

        /**
         * @brief Adds the supplied value to the current aggregate.
         *
         * @param[in] num Number of profiler entries/counters addressed by the operation.
         */
        inline void add(int num = 1) {
            count += num;
        }

        /**
         * @brief Pointer to desc; `nullptr` denotes that no object/resource is assigned.
         */
        const char *desc; ///< Pointer to desc; `nullptr` denotes that no object/resource is assigned.
        uint32_t count; ///< Number of samples accumulated in this profiling counter.
    };

private:
    /**
     * @brief Maximum supported intervals.
     */
    static const int MaxIntervals = 16; ///< Maximum supported intervals.
    /**
     * @brief Maximum supported counters.
     */
    static const int MaxCounters = 16; ///< Maximum supported counters.

    /**
     * @brief Registers interval.
     *
     * @param[in] interval Interval or duration in the unit defined by the owning API.
     */
    static void registerInterval(Interval *interval);
    /**
     * @brief Registers counter.
     *
     * @param[in] counter Profiler counter descriptor/value to register or update.
     */
    static void registerCounter(Counter *counter);

    /**
     * @brief Runtime value representing num intervals.
     */
    static int _numIntervals; ///< Number of profiler intervals currently registered.
    /**
     * @brief Runtime value representing num counters.
     */
    static int _numCounters; ///< Number of profiler counters currently registered.
    /**
     * @brief Pointer to intervals; `nullptr` denotes that no object/resource is assigned.
     */
    static Interval *_intervals[MaxIntervals]; ///< Pointer to intervals; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Pointer to counters; `nullptr` denotes that no object/resource is assigned.
     */
    static Counter *_counters[MaxCounters]; ///< Pointer to counters; `nullptr` denotes that no object/resource is assigned.
};

# define PROFILER_INTERVAL(_name_, _desc_) \
    /**
     * @brief Performs the _profiler_interval operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static Profiler::Interval _name_##_profiler_interval(_desc_);
# define PROFILER_INTERVAL_BEGIN(_name_) \
    /**
     * @brief Performs the begin operation.
     *
     * @return Computed result in the domain described by this function.
     */
    _name_##_profiler_interval.begin();
# define PROFILER_INTERVAL_END(_name_) \
    /**
     * @brief Performs the end operation.
     *
     * @return Computed result in the domain described by this function.
     */
    _name_##_profiler_interval.end();

# define PROFILER_COUNTER(_name_, _desc_) \
    /**
     * @brief Performs the _profiler_counter operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static Profiler::Counter _name_##_profiler_counter(_desc_);
# define PROFILER_COUNTER_ADD(_num_) \
    /**
     * @brief Performs the add operation.
     *
     * @return Computed result in the domain described by this function.
     */
    _name_##_profiler_counter.add(_num_);

#else // CONFIG_ENABLE_PROFILER

/**
 * @brief Implements profiler operations and maintains the associated runtime state.
 */
class Profiler {
public:
    /**
     * @brief Initializes the Profiler and its runtime resources.
     */
    static void init() {}
    /**
     * @brief Writes diagnostic state to the configured debug output.
     */
    static void dump() {}
};

# define PROFILER_INTERVAL(_name_, _desc_)
# define PROFILER_INTERVAL_BEGIN(_name_)
# define PROFILER_INTERVAL_END(_name_)

# define PROFILER_COUNTER(_name_, _desc_)
# define PROFILER_COUNTER_ADD(_num_)

#endif // CONFIG_ENABLE_PROFILER
