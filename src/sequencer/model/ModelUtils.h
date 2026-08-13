/**
 * @file ModelUtils.h
 * @brief Declares the ModelUtils component used by the sequencer model.
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

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <algorithm>
#include <bitset>

namespace ModelUtils {

/**
 * @brief Clamps an enum value to the valid `[0, Last)` domain.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 *
 * @return Computed result in the domain described by this function.
 */
template<typename Enum>
static Enum clampedEnum(Enum value) {
    return Enum(clamp(int(value), 0, int(Enum::Last) - 1));
}

/**
 * @brief Applies an integer edit offset and clamps the result to the supplied range.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] offset Signed edit offset applied to the current value.
 * @param[in] min Inclusive minimum permitted result.
 * @param[in] max Inclusive maximum permitted result.
 *
 * @return Computed result in the domain described by this function.
 */
static int adjusted(int value, int offset, int min, int max) {
    return clamp(value + offset, min, max);
}

/**
 * @brief Applies an integer edit offset to an enum and clamps the result to its valid domain.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] offset Signed edit offset applied to the current value.
 *
 * @return Computed result in the domain described by this function.
 */
template<typename Enum>
static Enum adjustedEnum(Enum value, int offset) {
    return Enum(adjusted(int(value), offset, 0, int(Enum::Last) - 1));
}

/**
 * @brief Applies an edit offset using the supplied step size and Shift modifier.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] offset Signed edit offset applied to the current value.
 * @param[in] step Edit step size or zero-based sequence step as defined by the operation.
 * @param[in] shift Whether the Shift modifier requests the coarse/alternate edit step.
 *
 * @return Computed result in the domain described by this function.
 */
int adjustedByStep(int value, int offset, int step, bool shift);
/**
 * @brief Adjusts an integer through the supported power-of-two value sequence.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] offset Signed edit offset applied to the current value.
 * @param[in] shift Whether the Shift modifier requests the coarse/alternate edit step.
 *
 * @return Computed result in the domain described by this function.
 */
int adjustedByPowerOfTwo(int value, int offset, bool shift);
/**
 * @brief Adjusts a clock/sequence divisor through the supported divisor sequence.
 *
 * @param[in] value Input value to clamp, adjust, format or convert.
 * @param[in] offset Signed edit offset applied to the current value.
 * @param[in] shift Whether the Shift modifier requests the coarse/alternate edit step.
 *
 * @return Computed result in the domain described by this function.
 */
int adjustedByDivisor(int value, int offset, bool shift);

/**
 * @brief Formats a boolean value as the localized Yes/No label.
 *
 * @param[in,out] str String builder that receives the formatted representation.
 * @param[in] value Input value to clamp, adjust, format or convert.
 */
void printYesNo(StringBuilder &str, bool value);
/**
 * @brief Formats a sequencer divisor as a musical note-length label.
 *
 * @param[in,out] str String builder that receives the formatted representation.
 * @param[in] value Input value to clamp, adjust, format or convert.
 */
void printDivisor(StringBuilder &str, int value);

/**
 * @brief Maps a supported sequencer divisor to its known-divisor table index.
 *
 * @param[in] divisor Sequencer timing divisor in the model-defined divisor domain.
 *
 * @return Computed result in the domain described by this function.
 */
int divisorToIndex(int divisor);
/**
 * @brief Maps a known-divisor table index back to its sequencer divisor.
 *
 * @param[in] index Zero-based table/matrix index addressed by the operation.
 *
 * @return Computed result in the domain described by this function.
 */
int indexToDivisor(int index);

/**
 * @brief Clamps a sequencer divisor to the model-supported range.
 *
 * @param[in] divisor Sequencer timing divisor in the model-defined divisor domain.
 *
 * @return Computed result in the domain described by this function.
 */
static int clampDivisor(int divisor) {
    return clamp(divisor, 1, 768);
}

/**
 * @brief Moves sequence-step data left or right within the addressed selection/range.
 *
 * @param[in] steps Fixed-size sequence-step array to modify.
 * @param[in] direction Shift direction: positive/right or negative/left according to the caller contract.
 */
template<typename Step, size_t N>
static void shiftSteps(std::array<Step, N> &steps, int direction) {
    if (direction == 1) {
        for (int i = int(steps.size()) - 2; i >= 0; --i) {
            std::swap(steps[i], steps[i + 1]);
        }
    } else if (direction == -1) {
        for (int i = 0; i < int(steps.size()) - 1; ++i) {
            std::swap(steps[i], steps[i + 1]);
        }
    }
}

/**
 * @brief Moves sequence-step data left or right within the addressed selection/range.
 *
 * @param[in] steps Fixed-size sequence-step array to modify.
 * @param[in] first First inclusive step/index in the addressed range.
 * @param[in] last Last inclusive step/index in the addressed range.
 * @param[in] direction Shift direction: positive/right or negative/left according to the caller contract.
 */
template<typename Step, size_t N>
static void shiftSteps(std::array<Step, N> &steps, int first, int last, int direction)
{
    if (direction == 1) {
        for (int i = last - 1; i >= first; --i) {
            std::swap(steps[i], steps[i + 1]);
        }
    } else if (direction == -1) {
        for (int i = first; i < last; ++i) {
            std::swap(steps[i], steps[i + 1]);
        }
    }
}

/**
 * @brief Moves sequence-step data left or right within the addressed selection/range.
 *
 * @param[in] steps Fixed-size sequence-step array to modify.
 * @param[in] selected Bit set identifying the steps selected for the operation.
 * @param[in] direction Shift direction: positive/right or negative/left according to the caller contract.
 */
template<typename Step, size_t N>
static void shiftSteps(std::array<Step, N> &steps, const std::bitset<N> &selected, int direction)
{
    uint8_t indices[N];
    int count = 0;
    for (size_t i = 0; i < N; ++i) {
        if (selected[i]) indices[count++] = i;
    }
    if (direction == 1) {
        for (int i = count - 2; i >= 0; --i) {
            std::swap(steps[indices[i]], steps[indices[i + 1]]);
        }
    } else if (direction == -1) {
        for (int i = 0; i < count - 1; ++i) {
            std::swap(steps[indices[i]], steps[indices[i + 1]]);
        }
    }
}

/**
 * @brief Copies a contiguous step range into the immediately following range when space permits.
 *
 * @param[in] steps Fixed-size sequence-step array to modify.
 * @param[in] firstStep First inclusive active sequence-step index.
 * @param[in] lastStep Last inclusive active sequence-step index.
 */
template<typename Step, size_t N>
static void duplicateSteps(std::array<Step, N> &steps, int firstStep, int lastStep) {
    for (int src = firstStep; src <= lastStep; ++src) {
        int dst = src + (lastStep - firstStep + 1);
        if (dst < int(steps.size())) {
            steps[dst] = steps[src];
        }
    }
}

/**
 * @brief Copies source steps into selected destination positions, cycling selected sources as needed.
 *
 * @param[in] src Source step array.
 * @param[in] srcSelected Bit set selecting source steps; an empty selection means all source steps.
 * @param[in,out] dst Destination step array to update.
 * @param[in] dstSelected Bit set selecting destination steps; an empty selection means all destination steps.
 */
template<typename Step, size_t N>
static void copySteps(
    const std::array<Step, N> &src, const std::bitset<N> &srcSelected,
    std::array<Step, N> &dst, const std::bitset<N> &dstSelected
) {
    auto nextSelected = [] (const std::bitset<N> &selected, int index) {
        if (selected.none()) {
            return (index + 1) % int(N);
        } else {
            do {
                index = (index + 1) % int(N);
            } while (!selected[index]);
            return index;
        }
    };

    int srcIndex = -1;

    for (size_t dstIndex = 0; dstIndex < N; ++dstIndex) {
        if (dstSelected.none() || dstSelected[dstIndex]) {
            srcIndex = nextSelected(srcSelected, srcIndex);
            dst[dstIndex] = src[srcIndex];
        }
    }
}

} // namespace ModelUtils
