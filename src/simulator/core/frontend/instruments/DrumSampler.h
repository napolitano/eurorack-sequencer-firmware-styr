/**
 * @file DrumSampler.h
 * @brief Declares the DrumSampler component used by the simulator frontend.
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

#include "../Audio.h"
#include "../Instrument.h"

namespace sim {

/**
 * @brief Provides drum sampler behavior for the desktop simulator.
 */
class DrumSampler : public Instrument {
public:
    /**
     * @brief Constructs a DrumSampler instance.
     *
     * @param[in] audio Audio source/buffer processed by the operation.
     * @param[in] filename File name/path used for the operation.
     */
    DrumSampler(Audio &audio, const std::string &filename);

    /**
     * @brief Sets the gate.
     *
     * @param[in] gate `true` to enable/set gate; `false` to disable/clear it.
     */
    virtual void setGate(bool gate) override;
    /**
     * @brief Sets the cv.
     *
     * @param[in] cv Control-voltage value in the engine/model voltage domain.
     */
    virtual void setCv(float cv) override;

private:
    /**
     * @brief Triggers the instrument/event.
     */
    void trigger();

    /**
     * @brief Reference to audio owned by another component.
     */
    Audio &_audio; ///< Reference to audio owned by another component.
    Sample _sample; ///< Selected sample that determines how `DrumSampler` interprets or renders its data.
    /**
     * @brief Whether gate is true in the current state.
     */
    bool _gate = false; ///< True when the generated/current step requests an active gate.
};

} // namespace sim
