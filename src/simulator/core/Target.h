/**
 * @file Target.h
 * @brief Declares the Target component used by the simulator core.
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

#include "EncoderEvent.h"
#include "MidiEvent.h"
#include "FrameBuffer.h"

#include <functional>

#include <cstdint>

namespace sim {

/**
 * @brief Provides target behavior for the desktop simulator.
 */
struct Target {
    /**
     * @brief Returns the stored callable or opaque platform value.
     *
     * @return The void value.
     */
    std::function<void()> create; ///< Callback that creates and initializes the simulated target.
    /**
     * @brief Returns the stored callable or opaque platform value.
     *
     * @return The void value.
     */
    std::function<void()> destroy; ///< Callback that tears down the simulated target.
    /**
     * @brief Returns the stored callable or opaque platform value.
     *
     * @return The void value.
     */
    std::function<void()> update; ///< Callback that advances the simulated target by one host update cycle.
};

/**
 * @brief Provides target tick handler behavior for the desktop simulator.
 */
struct TargetTickHandler {
    /**
     * @brief Sets the tick.
     *
     * @param[in] tick Absolute sequencer engine tick at which the operation is evaluated.
     */
    virtual void setTick(uint32_t tick) {}
};

/**
 * @brief Provides target input handler behavior for the desktop simulator.
 */
struct TargetInputHandler {
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based write button index.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    virtual void writeButton(int index, bool pressed) {}
    /**
     * @brief Writes encoder.
     *
     * @param[in] event Event to process.
     */
    virtual void writeEncoder(EncoderEvent event) {}
    /**
     * @brief Writes adc.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write adc.
     */
    virtual void writeAdc(int channel, uint16_t value) {}
    /**
     * @brief Writes digital input.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital input.
     */
    virtual void writeDigitalInput(int pin, bool value) {}
    /**
     * @brief Writes midi input.
     *
     * @param[in] event Event to process.
     */
    virtual void writeMidiInput(MidiEvent event) {}
};

/**
 * @brief Provides target output handler behavior for the desktop simulator.
 */
struct TargetOutputHandler {
    /**
     * @brief Writes led.
     *
     * @param[in] index Zero-based write led index.
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     */
    virtual void writeLed(int index, bool red, bool green) {}
    /**
     * @brief Writes gate output.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write gate output.
     */
    virtual void writeGateOutput(int channel, bool value) {}
    /**
     * @brief Writes dac.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write dac.
     */
    virtual void writeDac(int channel, uint16_t value) {}
    /**
     * @brief Writes digital output.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital output.
     */
    virtual void writeDigitalOutput(int pin, bool value) {}
    /**
     * @brief Writes lcd.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    virtual void writeLcd(const FrameBuffer &frameBuffer) {}
    /**
     * @brief Writes midi output.
     *
     * @param[in] event Event to process.
     */
    virtual void writeMidiOutput(MidiEvent event) {}
};

} // namespace sim
