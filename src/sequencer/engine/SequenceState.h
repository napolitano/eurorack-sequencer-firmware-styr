/**
 * @file SequenceState.h
 * @brief Declares the SequenceState component used by the sequencer engine.
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

#include "model/Types.h"

#include "core/utils/Random.h"

#include <cstdint>

/**
 * @brief Stores sequence state used while the component is running.
 */
class SequenceState {
public:
    /**
     * @brief Advances the SequenceState by one logical step.
     *
     * @return Zero-based sequence-step index represented by the key/state.
     */
    int step() const { return _step; }
    /**
     * @brief Returns the prev step.
     *
     * @return Previously visited sequence-step index.
     */
    int prevStep() const { return _prevStep; }
    /**
     * @brief Returns the direction.
     *
     * @return Current sequence traversal direction.
     */
    int direction() const { return _direction; }
    /**
     * @brief Returns the iteration.
     *
     * @return Current sequence iteration counter.
     */
    uint32_t iteration() const { return _iteration; }

    /**
     * @brief Resets the SequenceState to its initial runtime state.
     */
    void reset();

    /**
     * @brief Advances free.
     *
     * @param[in] runMode Sequence run mode controlling traversal order.
     * @param[in] firstStep Zero-based first active sequence-step index.
     * @param[in] lastStep Zero-based last active sequence-step index.
     * @param[in] rng Random-number generator used for deterministic/randomized selection.
     */
    void advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng);
    /**
     * @brief Advances aligned.
     *
     * @param[in] absoluteStep Absolute transport step used to resolve sequence-relative position.
     * @param[in] runMode Sequence run mode controlling traversal order.
     * @param[in] firstStep Zero-based first active sequence-step index.
     * @param[in] lastStep Zero-based last active sequence-step index.
     * @param[in] rng Random-number generator used for deterministic/randomized selection.
     */
    void advanceAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng);

private:
    /**
     * @brief Advances random walk.
     *
     * @param[in] firstStep Zero-based first active sequence-step index.
     * @param[in] lastStep Zero-based last active sequence-step index.
     * @param[in] rng Random-number generator used for deterministic/randomized selection.
     */
    void advanceRandomWalk(int firstStep, int lastStep, Random &rng);

    /**
     * @brief Zero-based sequence-step index associated with this state/event.
     */
    int8_t _step; ///< Zero-based sequence-step index associated with this state/event.
    /**
     * @brief Previous step retained for comparison.
     */
    int8_t _prevStep; ///< Previous step retained for comparison.
    /**
     * @brief Runtime value representing direction.
     */
    int8_t _direction; ///< Direction selector or signed direction stored by this state.
    /**
     * @brief Sequence traversal iteration associated with this state/reservation.
     */
    uint32_t _iteration; ///< Sequence traversal iteration associated with this state/reservation.
};
