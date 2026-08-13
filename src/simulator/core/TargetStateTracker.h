/**
 * @file TargetStateTracker.h
 * @brief Declares the TargetStateTracker component used by the simulator core.
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

#include "Target.h"
#include "TargetState.h"

namespace sim {

/**
 * @brief Provides target state tracker behavior for the desktop simulator.
 */
class TargetStateTracker : public TargetInputHandler, public TargetOutputHandler {
public:
    /**
     * @brief Constructs a TargetStateTracker instance.
     *
     * @param[in] targetState Target-state snapshot consumed by the tracker.
     */
    TargetStateTracker(TargetState &targetState);

    /**
     * @brief Returns the target state.
     *
     * @return Reference to the target state.
     */
    TargetState &targetState() { return _targetState; }

    // TargetInputHandler
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based write button index.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    virtual void writeButton(int index, bool pressed) override;
    /**
     * @brief Writes adc.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write adc.
     */
    virtual void writeAdc(int channel, uint16_t value) override;
    /**
     * @brief Writes digital input.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital input.
     */
    virtual void writeDigitalInput(int pin, bool value) override;

    // TargetOutputHandler
    /**
     * @brief Writes led.
     *
     * @param[in] index Zero-based write led index.
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     */
    virtual void writeLed(int index, bool red, bool green) override;
    /**
     * @brief Writes gate output.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write gate output.
     */
    virtual void writeGateOutput(int channel, bool value) override;
    /**
     * @brief Writes dac.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write dac.
     */
    virtual void writeDac(int channel, uint16_t value) override;
    /**
     * @brief Writes digital output.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital output.
     */
    virtual void writeDigitalOutput(int pin, bool value) override;
    /**
     * @brief Writes lcd.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    virtual void writeLcd(const FrameBuffer &frameBuffer) override;

private:
    /**
     * @brief Reference to target state owned by another component.
     */
    TargetState &_targetState; ///< Reference to target state owned by another component.
};

} // namespace sim
