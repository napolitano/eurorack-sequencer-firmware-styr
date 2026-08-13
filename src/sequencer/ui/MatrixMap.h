/**
 * @file MatrixMap.h
 * @brief Declares the MatrixMap component used by the sequencer UI.
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

namespace MatrixMap {

    // index = col * 8 + row

    //                              ROW
    //          0     1     2     3     4     5     6     7
    //        -----------------------------------------------
    //     5 | F-0 | F-1 | F-2 | F-3 | F-4 |                 |
    //  C  4 | xxx | xxx | xxx | xxx | xxx | xxx | xxx | xxx |
    //  O  2 | T-0 | T-1 | T-2 | T-3 | T-4 | T-5 | T-6 | T-7 |
    //  L  1 | S-0 | S-1 | S-2 | S-3 | S-4 | S-5 | S-6 | S-7 |
    //     0 | S-8 | S-9 | S10 | S11 | S12 | S13 | S14 | S15 |
    //        -----------------------------------------------

    /**
     * @brief Maps a sequencer step index to its physical matrix index.
     *
     * @param[in] step Edit step size or zero-based sequence step as defined by the operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int fromStep(int step) {
        return step < 8 ? (8 + step) : (step - 8);
    }

    /**
     * @brief Reports whether a physical matrix index belongs to the step-key region.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return `true` when the documented condition is satisfied; otherwise `false`.
     */
    static constexpr bool isStep(int index) {
        return index >= fromStep(8) && index <= fromStep(7);
    }

    /**
     * @brief Maps a physical matrix index back to its sequencer step index.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int toStep(int index) {
        return index >= fromStep(0) ? (index - fromStep(0)) : (index - fromStep(8) + 8);
    }

    /**
     * @brief Maps a sequencer track index to its physical matrix index.
     *
     * @param[in] track Zero-based sequencer track index.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int fromTrack(int track) {
        return 16 + track;
    }

    /**
     * @brief Reports whether a physical matrix index belongs to the track-key region.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return `true` when the documented condition is satisfied; otherwise `false`.
     */
    static constexpr bool isTrack(int index) {
        return index >= fromTrack(0) && index <= fromTrack(7);
    }

    /**
     * @brief Maps a physical matrix index back to its sequencer track index.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int toTrack(int index) {
        return index - fromTrack(0);
    }

    /**
     * @brief Maps a function-key index to its physical matrix index.
     *
     * @param[in] function Zero-based hardware function-key index.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int fromFunction(int function) {
        return 32 + function;
    }

    /**
     * @brief Reports whether a physical matrix index belongs to the function-key region.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return `true` when the documented condition is satisfied; otherwise `false`.
     */
    static constexpr int isFunction(int index) {
        return index >= fromFunction(0) && index <= fromFunction(4);
    }

    /**
     * @brief Maps a physical matrix index back to its function-key index.
     *
     * @param[in] index Zero-based table/matrix index addressed by the operation.
     *
     * @return Computed result in the domain described by this function.
     */
    static constexpr int toFunction(int index) {
        return index - fromFunction(0);
    }

} // namespace MatrixMap
