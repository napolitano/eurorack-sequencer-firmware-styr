/**
 * @file TargetTraceRecorder.h
 * @brief Declares the TargetTraceRecorder component used by the simulator core.
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

#include "TargetStateTracker.h"
#include "TargetTrace.h"

namespace sim {

/**
 * @brief Provides target trace recorder behavior for the desktop simulator.
 */
class TargetTraceRecorder : public TargetStateTracker, public TargetTickHandler {
public:
    /**
     * @brief Constructs a TargetTraceRecorder instance.
     *
     * @param[in] targetTrace Trace/buffer receiving target-side diagnostic data.
     */
    TargetTraceRecorder(TargetTrace &targetTrace);

    /**
     * @brief Returns the target trace.
     *
     * @return Reference to the target trace.
     */
    TargetTrace &targetTrace() { return _targetTrace; }

    // TargetTickHandler
    /**
     * @brief Sets the tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     */
    virtual void setTick(uint32_t tick) override;

    // TargetInputHandler
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based write button index.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    virtual void writeButton(int index, bool pressed) override;
    /**
     * @brief Writes encoder.
     *
     * @param[in] event Event to process.
     */
    virtual void writeEncoder(EncoderEvent event) override;
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
    /**
     * @brief Writes midi input.
     *
     * @param[in] event Event to process.
     */
    virtual void writeMidiInput(MidiEvent event) override;

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
    /**
     * @brief Writes midi output.
     *
     * @param[in] event Event to process.
     */
    virtual void writeMidiOutput(MidiEvent event) override;

private:
    TargetState _targetState; ///< Last observed simulator target snapshot used to detect state changes for trace recording.
    /**
     * @brief Current absolute sequencer engine tick.
     */
    uint32_t _tick = 0; ///< Current absolute sequencer engine tick.
    /**
     * @brief Reference to target trace owned by another component.
     */
    TargetTrace &_targetTrace; ///< Reference to target trace owned by another component.
};

} // namespace sim
