/**
 * @file Synth.h
 * @brief Declares the Synth component used by the simulator frontend.
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

#include "soloud.h"

namespace sim {

class Synth;
class Voice;

/**
 * @brief Provides synth instance behavior for the desktop simulator.
 */
class SynthInstance : public SoLoud::AudioSourceInstance {
public:
    /**
     * @brief Constructs a SynthInstance instance.
     *
     * @param[in] synth SoLoud synth/audio object wrapped by the simulator instrument.
     */
    SynthInstance(Synth &synth);
    /**
     * @brief Destroys the SynthInstance instance.
     */
    virtual ~SynthInstance();

    /**
     * @brief Returns get audio.
     *
     * @param[in] aBuffer Audio output buffer to fill with generated samples.
     * @param[in] aSamplesToRead Number of audio samples requested by the callback.
     * @param[in] aBufferSize Capacity of the supplied audio buffer in samples.
     *
     * @return Result of getAudio().
     */
    virtual unsigned int getAudio(float *aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize) override;
    /**
     * @brief Reports whether the SynthInstance has ended.
     *
     * @return `true` if ended; otherwise `false`.
     */
    virtual bool hasEnded() override;

private:
    /**
     * @brief Reference to synth owned by another component.
     */
    Synth &_synth; ///< Reference to synth owned by another component.
    std::unique_ptr<Voice> _voice; ///< Owned synthesizer voice that generates this instrument output.
};

/**
 * @brief Provides synth behavior for the desktop simulator.
 */
class Synth : public Instrument, SoLoud::AudioSource {
public:
    /**
     * @brief Constructs a Synth instance.
     *
     * @param[in] audio Audio source/buffer processed by the operation.
     */
    Synth(Audio &audio);
    /**
     * @brief Destroys the Synth instance.
     */
    ~Synth();

    /**
     * @brief Returns the create instance.
     *
     * @return Pointer to the create instance; `nullptr` when no value is available.
     */
    virtual SoLoud::AudioSourceInstance *createInstance() override;

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
     * @brief Reference to audio owned by another component.
     */
    Audio &_audio; ///< Reference to audio owned by another component.
    /**
     * @brief Simulator value representing handle.
     */
    int _handle; ///< Audio-engine voice handle used to address the simulator synth instance.
    /**
     * @brief Whether gate is true in the current state.
     */
    bool _gate = false; ///< True when the generated/current step requests an active gate.
    /**
     * @brief Control-voltage value carried by this state/event.
     */
    float _cv = 0.f; ///< Control-voltage value carried by this state/event.

    friend class SynthInstance;
};

} // namespace sim
