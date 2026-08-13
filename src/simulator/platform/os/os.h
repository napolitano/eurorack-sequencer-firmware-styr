/**
 * @file os.h
 * @brief Declares the os component used by the simulator OS integration.
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

#include "core/Simulator.h"

#include <cstdint>
#include <functional>
#include <mutex>
#include <utility>

namespace os {

    using UpdateCallback = std::function<void(void)>;
    using UpdateCallbackId = uint64_t;

    /**
     * @brief Registers a callback that runs during each simulator update cycle.
     *
     * @param[in] callback Callback to register with the simulator update loop.
     *
     * @return Computed result in the domain described by this function.
     */
    UpdateCallbackId addUpdateCallback(UpdateCallback callback);
    /**
     * @brief Unregisters a previously registered simulator update callback.
     *
     * @param[in] id Identifier returned when the callback was registered.
     */
    void removeUpdateCallback(UpdateCallbackId id);
    /**
     * @brief Invokes the currently registered simulator update callbacks.
     */
    void runUpdateCallbacks();

    typedef int TaskHandle;

    /**
     * @brief Provides task behavior for the desktop simulator.
     */
    template<size_t StackSize>
    /**
     * @brief Provides task behavior for the desktop simulator.
     */
    class Task {
    public:
        /**
         * @brief Constructs a Task instance.
         *
         * @param[in] name Name or display string associated with the object.
         * @param[in] priority Task scheduling priority.
         * @param[in] func Function/callback invoked by the operation.
         */
        Task(const char *name, uint8_t priority, std::function<void(void)> func) :
            /**
             * @brief Returns the func.
             */
            _func(func)
        {
        }

        /**
         * @brief Returns the handle.
         *
         * @return Underlying RTOS/host handle owned by the wrapper.
         */
        TaskHandle handle() const { return 0; }

    private:
        /**
         * @brief Returns the stored callable or opaque platform value.
         *
         * @return The void value.
         */
        std::function<void(void)> _func; ///< Task entry callback executed by the simulator scheduler.
    };

    /**
     * @brief Provides periodic task behavior for the desktop simulator.
     */
    template<size_t StackSize>
    /**
     * @brief Provides periodic task behavior for the desktop simulator.
     */
    class PeriodicTask {
    public:
        /**
         * @brief Constructs a PeriodicTask instance.
         *
         * @param[in] name Name or display string associated with the object.
         * @param[in] priority Task scheduling priority.
         * @param[in] interval Interval or duration in the unit defined by the owning API.
         * @param[in] func Function/callback invoked by the operation.
         */
        PeriodicTask(const char *name, uint8_t priority, uint32_t interval, std::function<void(void)> func) :
            /**
             * @brief Returns the callback id.
             */
            _callbackId(os::addUpdateCallback(std::move(func)))
        {
        }

        /**
         * @brief Destroys the PeriodicTask instance.
         */
        ~PeriodicTask() {
            os::removeUpdateCallback(_callbackId);
        }

        /**
         * @brief Constructs a PeriodicTask instance.
         *
         * @note Includes an unnamed `const PeriodicTask &` input parameter as declared by the inherited/interface signature.
         */
        PeriodicTask(const PeriodicTask &) = delete;
        /**
         * @brief Disables copy assignment because the task owns a registered simulator callback.
         *
         * @return This operation is deleted and cannot be called.
         */
        PeriodicTask &operator=(const PeriodicTask &) = delete;

    private:
        UpdateCallbackId _callbackId; ///< Identifier used to unregister the simulator callback.
    };

    /**
     * @brief Suspends the addressed task.
     *
     * @param[in] handle Task handle addressed by the scheduler operation.
     */
    inline void suspend(TaskHandle handle) {}
    inline void resume(TaskHandle handle) {}
    inline void resumeFromISR(TaskHandle handle) {}

    namespace this_task {

        inline TaskHandle handle() { return -1; }

        inline void suspend() {}
        inline void resume() {}
        inline void yield() {}

    } // namespace this_task


    /**
     * @brief Provides semaphore generic behavior for the desktop simulator.
     */
    class SemaphoreGeneric {
    public:
        /**
         * @brief Returns take.
         *
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if take; otherwise `false`.
         */
        bool take(uint32_t timeToWait = -1) {
            return false;
        }

        /**
         * @brief Returns the give.
         *
         * @return `true` if give; otherwise `false`.
         */
        bool give() {
            return false;
        }

    protected:
        /**
         * @brief Constructs a SemaphoreGeneric instance.
         */
        SemaphoreGeneric() {}
    };

    /**
     * @brief Provides semaphore behavior for the desktop simulator.
     */
    class Semaphore : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a Semaphore instance.
         */
        Semaphore() {
            ASSERT(false, "not implemented");
        }
    };

    /**
     * @brief Provides counting semaphore behavior for the desktop simulator.
     */
    class CountingSemaphore : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a CountingSemaphore instance.
         *
         * @param[in] maxCount Maximum number of items/events permitted.
         * @param[in] initialCount Initial item/event count.
         */
        CountingSemaphore(uint32_t maxCount = -1, uint32_t initialCount = 0) {
            ASSERT(false, "not implemented");
        }
    };

    /**
     * @brief Provides mutex behavior for the desktop simulator.
     */
    class Mutex {
    public:
        /**
         * @brief Returns take.
         *
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if take; otherwise `false`.
         */
        bool take(uint32_t timeToWait = -1) {
            ASSERT(timeToWait == uint32_t(-1), "cannot wait for mutex");
            _mutex.lock();
            return true;
        }

        /**
         * @brief Returns the give.
         *
         * @return `true` if give; otherwise `false`.
         */
        bool give() {
            _mutex.unlock();
            return true;
        }

    private:
        std::mutex _mutex; ///< Host mutex implementing the simulator-side OS mutex abstraction.
    };

    /**
     * @brief Provides recursive mutex behavior for the desktop simulator.
     */
    class RecursiveMutex : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a RecursiveMutex instance.
         */
        RecursiveMutex() {
            ASSERT(false, "not implemented");
        }
    };

    /**
     * @brief Provides queue behavior for the desktop simulator.
     */
    template<typename T, size_t Length>
    /**
     * @brief Provides queue behavior for the desktop simulator.
     */
    class Queue {
    public:
        /**
         * @brief Constructs a Queue instance.
         */
        Queue() {
            ASSERT(false, "not implemented");
        }

        /**
         * @brief Sends the supplied data through this transport/interface.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void send(const T& element, uint32_t timeToWait = -1) {
        }

        /**
         * @brief Sends to back.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void sendToBack(const T& element, uint32_t timeToWait = -1) {
        }

        /**
         * @brief Sends to front.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void sendToFront(const T& element, uint32_t timeToWait = -1) {
        }

        /**
         * @brief Returns peek.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if peek; otherwise `false`.
         */
        bool peek(T * element, uint32_t timeToWait = -1) {
            return false;
        }

        /**
         * @brief Returns receive.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if receive; otherwise `false`.
         */
        bool receive(T * element, uint32_t timeToWait = -1) {
            return false;
        }

        /**
         * @brief Returns the receive.
         *
         * @return Next value removed from the queue.
         */
        T receive() {
            return T(0);
        }

    private:
        T _data[Length]; ///< Owned fixed-size collection of data.
    };

    /**
     * @brief Provides interrupt lock behavior for the desktop simulator.
     */
    class InterruptLock {
    public:
        /**
         * @brief Constructs a InterruptLock instance.
         */
        InterruptLock() {}
        /**
         * @brief Destroys the InterruptLock instance.
         */
        ~InterruptLock() {}
    };

    namespace time {
        constexpr inline uint32_t us(uint32_t us) {
            return us / 1000;
        }
        constexpr inline uint32_t ms(uint32_t ms) {
            return ms;
        }
    };

    inline uint32_t ticks() {
        return sim::Simulator::instance().ticks();
    }

    inline void delay(uint32_t ticks) {
        uint32_t wakeupTick = os::ticks() + ticks;
        while (os::ticks() < wakeupTick) {}
    }

    inline void delayUntil(uint32_t &lastWakeupTime, uint32_t ticks) {
        uint32_t wakeupTick = os::ticks() + ticks;
        while (os::ticks() < wakeupTick) {}
    }

    inline void startScheduler() {
    }

} // namespace os
