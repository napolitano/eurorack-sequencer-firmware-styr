/**
 * @file os.h
 * @brief Declares the os component used by the STM32 sequencer OS integration.
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

#include "core/Debug.h"

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
}

#include <functional>

#include <libopencm3/cm3/cortex.h>

namespace os {

    typedef TaskHandle_t TaskHandle;

    /**
     * @brief Implements task profiler operations and maintains the associated runtime state.
     */
    class TaskProfiler {
    public:
        /**
         * @brief Describes metadata associated with the enclosing component.
         */
        struct TaskInfo {
            /**
             * @brief Pointer to next; `nullptr` denotes that no object/resource is assigned.
             */
            struct TaskInfo *next = nullptr; ///< Pointer to next; `nullptr` denotes that no object/resource is assigned.
            TaskHandle handle; ///< Underlying OS/task/resource handle.
            /**
             * @brief Size of stack in bytes/elements as defined by this type.
             */
            uint16_t stackSize; ///< Size of stack in bytes/elements as defined by this type.
            /**
             * @brief Most recently observed run time counter.
             */
            uint32_t lastRunTimeCounter; ///< Previous FreeRTOS runtime counter sample used to compute per-task execution deltas.
            /**
             * @brief Runtime value representing run time.
             */
            uint32_t runTime; ///< Elapsed runtime used by the component.
            /**
             * @brief Runtime value representing relative run time.
             */
            uint32_t relativeRunTime; ///< Runtime measured relative to the current local origin.
        };

        /**
         * @brief Registers task.
         *
         * @param[in] taskInfo Task descriptor receiving scheduler/runtime information.
         */
        static void registerTask(TaskInfo *taskInfo) {
            TaskInfo **tail = &_taskInfos;
            while (*tail != nullptr) {
                tail = &(*tail)->next;
            }
            *tail = taskInfo;
        }

        /**
         * @brief Writes diagnostic state to the configured debug output.
         */
        static void dump();

    private:
        template<typename Func>
        /**
         * @brief Enumerates available devices/resources.
         *
         * @param[in] func Function/callback invoked by the operation.
         */
        static void enumerate(Func func) {
            TaskInfo *info = _taskInfos;
            while (info) {
                func(*info);
                info = info->next;
            }
            _idleTaskInfo.handle = xTaskGetIdleTaskHandle();
            func(_idleTaskInfo);
        }

        /**
         * @brief Pointer to task infos; `nullptr` denotes that no object/resource is assigned.
         */
        static TaskInfo *_taskInfos; ///< Pointer to task infos; `nullptr` denotes that no object/resource is assigned.
        static TaskInfo _idleTaskInfo; ///< Metadata record representing the FreeRTOS idle task in task diagnostics.
    };

    /**
     * @brief Implements task operations and maintains the associated runtime state.
     */
    template<size_t StackSize>
    /**
     * @brief Wraps a statically allocated FreeRTOS task and its entry callback.
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
            _handle = xTaskCreateStatic(&start, name, StackSize / sizeof(StackType_t), this, priority, _stack, &_task);

#if CONFIG_ENABLE_TASK_PROFILER
            _taskInfo.handle = _handle;
            _taskInfo.stackSize = StackSize;
            TaskProfiler::registerTask(&_taskInfo);
#endif // CONFIG_ENABLE_TASK_PROFILER
        }

        /**
         * @brief Returns the handle.
         *
         * @return Underlying RTOS/host handle owned by the wrapper.
         */
        TaskHandle handle() const { return _handle; }

        /**
         * @brief Returns the name.
         *
         * @return Pointer to the name; `nullptr` when no value is available.
         */
        const char *name() const { return pcTaskGetName(_handle); }

        /**
         * @brief Returns the stack size.
         *
         * @return Number of pages currently present on the page stack.
         */
        size_t stackSize() const {
            return StackSize;
        }

    private:
        /**
         * @brief Starts the Task or operation.
         *
         * @param[in] task Task handle or task object addressed by the OS operation.
         */
        static void start(void *task) {
            reinterpret_cast<Task<StackSize> *>(task)->_func();
        }

        /**
         * @brief Returns the stored callable or opaque platform value.
         *
         * @return The void value.
         */
        std::function<void(void)> _func; ///< Task entry callback executed by FreeRTOS.
        TaskHandle_t _handle; ///< Underlying OS/task/resource handle.
        StaticTask_t _task; ///< Static RTOS task control block.
        /**
         * @brief Returns the sizeof.
         *
         * @return The sizeof value.
         */
        StackType_t _stack[StackSize / sizeof(StackType_t)];

#if CONFIG_ENABLE_TASK_PROFILER
        TaskProfiler::TaskInfo _taskInfo; ///< Profiler metadata associated with the task.
#endif // CONFIG_ENABLE_TASK_PROFILER

    };

    /**
     * @brief Suspends the addressed task.
     *
     * @param[in] handle Task handle addressed by the scheduler operation.
     */
    inline void suspend(TaskHandle handle) { vTaskSuspend(handle); }
    inline void resume(TaskHandle handle) { vTaskResume(handle); }
    inline void resumeFromISR(TaskHandle handle) { xTaskResumeFromISR(handle); }

    namespace this_task {

        inline TaskHandle handle() { return xTaskGetCurrentTaskHandle(); }

        inline void suspend() { os::suspend(handle()); }
        inline void resume() { os::suspend(handle()); }
        inline void yield() { taskYIELD(); }

    } // namespace this_task


    typedef BaseType_t TaskWoken;

    namespace isr {
        inline void exit(TaskWoken taskWoken) {
            portYIELD_FROM_ISR(taskWoken);
        }
    }


    /**
     * @brief Implements semaphore generic operations and maintains the associated runtime state.
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
        bool take(uint32_t timeToWait = portMAX_DELAY) {
            return xSemaphoreTake(_handle, timeToWait) == pdTRUE;
        }

        /**
         * @brief Returns the take from isr.
         *
         * @return `true` when the semaphore was acquired from ISR context; otherwise `false`.
         */
        TaskWoken takeFromISR() {
            TaskWoken taskWoken;
            xSemaphoreTakeFromISR(_handle, &taskWoken);
            return taskWoken;
        }

        /**
         * @brief Returns the give.
         *
         * @return `true` if give; otherwise `false`.
         */
        bool give() {
            return xSemaphoreGive(_handle) == pdTRUE;
        }

        /**
         * @brief Returns the give from isr.
         *
         * @return `true` when the semaphore was released from ISR context; otherwise `false`.
         */
        TaskWoken giveFromISR() {
            TaskWoken taskWoken;
            xSemaphoreGiveFromISR(_handle, &taskWoken);
            return taskWoken;
        }

        // ~SemaphoreGeneric() {
        //     vSemaphoreDelete(_handle);
        // }
    protected:
        /**
         * @brief Constructs a SemaphoreGeneric instance.
         */
        SemaphoreGeneric() {}

        SemaphoreHandle_t _handle; ///< Underlying OS/task/resource handle.
        StaticSemaphore_t _semaphore; ///< Static FreeRTOS semaphore control block; no heap allocation is required.
    };

    /**
     * @brief Implements semaphore operations and maintains the associated runtime state.
     */
    class Semaphore : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a Semaphore instance.
         */
        Semaphore() {
            _handle = xSemaphoreCreateBinaryStatic(&_semaphore);
        }
    };

    /**
     * @brief Implements counting semaphore operations and maintains the associated runtime state.
     */
    class CountingSemaphore : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a CountingSemaphore instance.
         *
         * @param[in] maxCount Maximum number of items/events permitted.
         * @param[in] initialCount Initial item/event count.
         */
        CountingSemaphore(uint32_t maxCount = portMAX_DELAY, uint32_t initialCount = 0) {
            _handle = xSemaphoreCreateCountingStatic(maxCount, initialCount, &_semaphore);
        }
    };

    /**
     * @brief Implements mutex operations and maintains the associated runtime state.
     */
    class Mutex : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a Mutex instance.
         */
        Mutex() {
            _handle = xSemaphoreCreateMutexStatic(&_semaphore);
        }
    };

    /**
     * @brief Implements recursive mutex operations and maintains the associated runtime state.
     */
    class RecursiveMutex : public SemaphoreGeneric {
    public:
        /**
         * @brief Constructs a RecursiveMutex instance.
         */
        RecursiveMutex() {
            _handle = xSemaphoreCreateRecursiveMutexStatic(&_semaphore);
        }
    };

    /**
     * @brief Implements queue operations and maintains the associated runtime state.
     */
    template<typename T, size_t Length>
    /**
     * @brief Wraps a statically allocated FreeRTOS queue with typed send/receive operations.
     */
    class Queue {
    public:
        /**
         * @brief Constructs a Queue instance.
         */
        Queue() {
            _handle = xQueueCreateStatic(Length, sizeof(T), reinterpret_cast<uint8_t *>(_data), &_queue);
        }

        /**
         * @brief Sends the supplied data through this transport/interface.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void send(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSend(_handle, &element, timeToWait);
        }

        /**
         * @brief Sends to back.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void sendToBack(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSendToBack(_handle, &element, timeToWait);
        }

        /**
         * @brief Sends to front.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         */
        void sendToFront(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSendToFront(_handle, &element, timeToWait);
        }

        /**
         * @brief Returns peek.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if peek; otherwise `false`.
         */
        bool peek(T * element, uint32_t timeToWait = portMAX_DELAY) {
            return xQueuePeek(_handle, element, timeToWait);
        }

        /**
         * @brief Returns receive.
         *
         * @param[in] element Element read, written, or transformed by the operation.
         * @param[in] timeToWait Maximum wait duration in the platform time unit used by the API.
         *
         * @return `true` if receive; otherwise `false`.
         */
        bool receive(T * element, uint32_t timeToWait = portMAX_DELAY) {
            return xQueueReceive(_handle, element, timeToWait);
        }

        /**
         * @brief Returns the receive.
         *
         * @return Next value removed from the queue.
         */
        T receive() {
            T element;
            xQueueReceive(_handle, &element, portMAX_DELAY);
            return element;
        }

    private:
        xQueueHandle _handle; ///< Underlying OS/task/resource handle.
        StaticQueue_t _queue; ///< Static FreeRTOS queue control block; no heap allocation is required.
        T _data[Length]; ///< Owned fixed-size collection of data.
    };

    /**
     * @brief Temporarily masks interrupts for a scoped bootloader critical section.
     */
    class InterruptLock {
    public:
        /**
         * @brief Constructs a InterruptLock instance.
         */
        InterruptLock() {
            cm_disable_interrupts();
            ++_nestedCount;
        }

        /**
         * @brief Destroys the InterruptLock instance.
         */
        ~InterruptLock() {
            if (--_nestedCount == 0) {
                cm_enable_interrupts();
            }
        }

    private:
        /**
         * @brief Number of nested items currently tracked or supported.
         */
        static uint32_t _nestedCount; ///< Number of nested items currently tracked or supported.
    };

    namespace time {
        constexpr inline uint32_t us(uint32_t us) {
            return (us * configTICK_RATE_HZ) / 1000000;
        }
        constexpr inline uint32_t ms(uint32_t ms) {
            return (ms * configTICK_RATE_HZ) / 1000;
        }
    };

    inline uint32_t ticks() {
        return xTaskGetTickCount();
    }

    inline void delay(uint32_t ticks) {
        vTaskDelay(ticks);
    }

    inline void delayUntil(uint32_t &lastWakeupTime, uint32_t ticks) {
        vTaskDelayUntil(&lastWakeupTime, ticks);
    }

    inline void startScheduler() {
        vTaskStartScheduler();
    }


    /**
     * @brief Implements periodic task operations and maintains the associated runtime state.
     */
    template<size_t StackSize>
    /**
     * @brief Wraps a periodic FreeRTOS task that invokes a callback at a configured interval.
     */
    class PeriodicTask : public Task<StackSize> {
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
            Task<StackSize>(name, priority, [interval, func] () {
                uint32_t lastWakeupTime = os::ticks();
                while (true) {
                    func();
                    os::delayUntil(lastWakeupTime, interval);
                }
            })
        {
        }
    };

} // namespace os
