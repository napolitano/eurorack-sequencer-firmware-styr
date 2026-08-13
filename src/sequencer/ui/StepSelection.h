/**
 * @file StepSelection.h
 * @brief Declares the StepSelection component used by the sequencer UI.
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

#include "Key.h"

#include <bitset>
#include <functional>

#include <cstdint>
#include <cstdlib>

/**
 * @brief Tracks a set/range of selected sequence steps and selection operations.
 */
template<size_t N>
class StepSelection {
public:
    /**
     * @brief Reports whether a key is currently held.
     *
     * @param[in] event Event to process.
     * @param[in] stepOffset Signed step offset applied to sequence positions.
     */
    void keyDown(KeyEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier()) {
            return;
        }

        if (key.isStep()) {
            if (_mode == Mode::Immediate && key.shiftModifier()) {
                // switch to persist mode
                _selected.reset();
                _mode = Mode::Persist;
                _first = -1;
            }
            if (_mode == Mode::Persist && !key.shiftModifier()) {
                // switch to immediate mode
                _selected.reset();
                _mode = Mode::Immediate;
                _first = -1;
            }

            int stepIndex = stepOffset + key.step();

            if (_selected.none()) {
                _first = stepIndex;
            }

            _selected.flip(stepIndex);

            event.consume();
        }
    }

    /**
     * @brief Reports whether a key is currently released.
     *
     * @param[in] event Event to process.
     * @param[in] stepOffset Signed step offset applied to sequence positions.
     */
    void keyUp(KeyEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier()) {
            return;
        }

        if (key.isStep() && _mode == Mode::Immediate) {
            int stepIndex = stepOffset + key.step();

            _selected.reset(stepIndex);

            if (_first >= 0 && !_selected[_first]) {
                _first = firstSetIndex();
            }

            event.consume();
        }
    }

    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     * @param[in] stepOffset Signed step offset applied to sequence positions.
     */
    void keyPress(KeyPressEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier() || !key.shiftModifier()) {
            return;
        }

        if (key.isShift() && event.count() == 2) {
            if (none()) {
                selectAll();
            } else if (_mode == Mode::Persist) {
                clear();
            }
            event.consume();
        }

        if (key.isStep() && event.count() == 2 && _mode == Mode::Persist) {
            int stepIndex = stepOffset + key.step();
            int otherStep = otherPressedStepKey(key.state(), key.step());
            if (otherStep >= 0) {
                int firstIndex = stepOffset + otherStep;
                int interval = stepIndex - firstIndex;
                for (int i = firstIndex; i >= 0 && i < int(_selected.size()); i += interval) {
                    _selected.set(i);
                }
                event.consume();
            } else {
                selectEqualSteps(stepIndex);
                event.consume();
            }
        }
    }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        _selected.reset();
        _mode = Mode::Immediate;
        _first = -1;
    }

    /**
     * @brief Selects all.
     */
    void selectAll() {
        _selected.set();
        _mode = Mode::Persist;
        _first = 0;
    }

    /**
     * @brief Selects equal steps.
     *
     * @param[in] stepIndex Zero-based step index.
     */
    void selectEqualSteps(int stepIndex) {
        _mode = Mode::Persist;
        for (int i = 0; i < int(_selected.size()); ++i) {
            if (i == stepIndex || (_stepCompare && _stepCompare(stepIndex, i))) {
                _selected.set(i);
            }
        }
    }

    /**
     * @brief Sets the step compare.
     *
     * @param[in] stepCompare Step compare consumed by `setStepCompare()`.
     */
    void setStepCompare(std::function<bool(int, int)> stepCompare) {
        _stepCompare = stepCompare;
    }

    /**
     * @brief Reports whether persisted.
     *
     * @return `true` if persisted; otherwise `false`.
     */
    bool isPersisted() const {
        return _mode == Mode::Persist;
    }

    /**
     * @brief Returns the first.
     *
     * @return First selected step index.
     */
    int first() const {
        return _first;
    }

    /**
     * @brief Returns the none.
     *
     * @return `true` if none; otherwise `false`.
     */
    bool none() const {
        return _selected.none();
    }

    /**
     * @brief Returns the any.
     *
     * @return `true` if any; otherwise `false`.
     */
    bool any() const {
        return _selected.any();
    }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const {
        return _selected.size();
    }

    /**
     * @brief Returns the count.
     *
     * @return Number of count represented by this object.
     */
    size_t count() const {
        return _selected.count();
    }

    /**
     * @brief Selects ed.
     *
     * @return Reference to the selected.
     */
    const std::bitset<N> &selected() const { return _selected; }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] index Zero-based component/element index.
     *
     * @return `true` if operator[]; otherwise `false`.
     */
    bool operator[](int index) const {
        return _selected[index];
    }

private:
    /**
     * @brief Returns other pressed step key.
     *
     * @param[in] keyState Current key/button state used by the UI operation.
     * @param[in] step Zero-based sequence-step index.
     *
     * @return Key identifier of another currently pressed step key, or the no-key sentinel.
     */
    int otherPressedStepKey(const KeyState &keyState, int step) const {
        bool found = false;
        int other = -1;
        for (int i = 0; i < 16; ++i) {
            if (i != step && keyState[MatrixMap::fromStep(i)]) {
                if (found) {
                    return -1;
                } else {
                    other = i;
                    found = true;
                }
            }
        }
        return other;
    }

    /**
     * @brief Returns the first set index.
     *
     * @return Zero-based first set index.
     */
    int firstSetIndex() const {
        for (size_t i = 0; i < _selected.size(); ++i) {
            if (_selected[i]) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Immediate, ///< Selects the immediate mode.
        Persist, ///< Selects the persist mode.
    };

    Mode _mode = Mode::Immediate; ///< Active mode controlling the behavior of `StepSelection`.
    std::bitset<N> _selected; ///< Bit mask of sequence steps currently included in the UI selection.
    /**
     * @brief UI value representing first.
     */
    int _first = -1; ///< First selected step index; `-1` means the selection is empty.
    /**
     * @brief Most recently observed pressed index.
     */
    int8_t _lastPressedIndex; ///< Most recent pressed step index, used as the anchor for range selection.
    /**
     * @brief Returns bool.
     *
     * @note Includes an unnamed `int` input parameter as declared by the inherited/interface signature.
     * @note Includes an unnamed `int` input parameter as declared by the inherited/interface signature.
     *
     * @return `true` when the addressed step belongs to the current selection; otherwise `false`.
     */
    std::function<bool(int, int)> _stepCompare;
};
