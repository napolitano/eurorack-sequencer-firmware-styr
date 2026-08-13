/**
 * @file SortedQueue.h
 * @brief Declares the SortedQueue component used by the sequencer engine.
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
#include <utility>
#include <functional>
#include <cstddef>

/**
 * @brief Stores fixed-capacity events in comparator-defined order without heap allocation.
 */
template<typename T, size_t Capacity, typename Compare = std::less<T>>
/**
 * @brief Stores a fixed-capacity queue ordered by the supplied comparator.
 */
class SortedQueue {
public:
    /**
     * @brief Constructs a SortedQueue instance.
     */
    SortedQueue() {
        clear();
    }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _read = 0;
        _write = 0;
    }

    /**
     * @brief Returns the capacity.
     *
     * @return Maximum number of elements the container can hold.
     */
    size_t capacity() {
        return Capacity;
    }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() {
        return (_write - _read) % Capacity;
    }

    /**
     * @brief Returns the empty.
     *
     * @return `true` if empty; otherwise `false`.
     */
    bool empty() const {
        return _write == _read;
    }

    /**
     * @brief Appends the supplied sample, item, or event.
     *
     * @param[in] value Value to push.
     */
    void push(const T &value) {
        insert(value);
    }

    /**
     * @brief Pushes replace.
     *
     * @param[in] value Value to push replace.
     */
    void pushReplace(const T &value) {
        size_t pos = insert(value);
        _write = increase(pos);
    }

    /**
     * @brief Returns the front.
     *
     * @return Reference to the front.
     */
    const T &front() const { return _queue[_read]; }
          /**
           * @brief Returns the front.
           *
           * @return Reference to the front.
           */
          T &front()       { return _queue[_read]; }

    /**
     * @brief Returns the back.
     *
     * @return Reference to the back.
     */
    const T &back() const { return _queue[_write]; }
          /**
           * @brief Returns the back.
           *
           * @return Reference to the back.
           */
          T &back()       { return _queue[_write]; }

    /**
     * @brief Removes and returns the next buffered item.
     */
    void pop() {
        if (size() > 0) {
            _read = increase(_read);
        }
    }

    /**
     * @brief Removes the last item from the container.
     */
    void popBack() {
        if (size() > 0) {
            _write = decrease(_write);
        }
    }

private:
    // insert into queue and return index of inserted value
    /**
     * @brief Returns insert.
     *
     * @param[in] value Value to insert.
     *
     * @return Result of insert().
     */
    size_t insert(const T &value) {
        Compare compare;
        // ensure we don't silently overflow circular buffer: if the next
        // write index would collide with _read, advance _read to drop the
        // oldest element and make room (behave like pushReplace).
        size_t nextWrite = increase(_write);
        if (nextWrite == _read) {
            _read = increase(_read); // drop oldest
        }
        size_t pos = _write;
        _queue[pos] = value;

        // insert sort
        size_t cur = _write;
        size_t prev = decrease(cur);
        while (cur != _read && compare(_queue[cur], _queue[prev])) {
            std::swap(_queue[cur], _queue[prev]);
            cur = prev;
            prev = decrease(cur);
        }

        _write = increase(_write);

        return cur;
    }

    /**
     * @brief Returns increase.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     *
     * @return Value after applying one increment.
     */
    inline size_t increase(size_t pos) const {
        return (pos + 1) % Capacity;
    }

    /**
     * @brief Returns decrease.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     *
     * @return Value after applying one decrement.
     */
    inline size_t decrease(size_t pos) const {
        return (pos - 1) % Capacity;
    }

    /**
     * @brief Fixed-capacity storage for queue.
     */
    std::array<T, Capacity> _queue; ///< Owned fixed-capacity queue entries used by `SortedQueue` without dynamic allocation.
    /**
     * @brief Runtime value representing read.
     */
    size_t _read; ///< Read cursor/index used by the ring buffer.
    /**
     * @brief Runtime value representing write.
     */
    size_t _write; ///< Write cursor/index used by the ring buffer.
};
