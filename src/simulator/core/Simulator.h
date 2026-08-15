/**
 * @file Simulator.h
 * @brief Declares the Simulator component used by the simulator core.
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
#include "TargetStateTracker.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class MidiMessage;

namespace sim {

/**
 * @brief Provides simulator behavior for the desktop simulator.
 */
class Simulator : public TargetInputHandler, public TargetOutputHandler {
public:
    /**
     * @brief Constructs a Simulator instance.
     *
     * @param[in] target Target object or value used by the operation.
     */
    Simulator(Target target);
    /**
     * @brief Destroys the Simulator instance.
     */
    virtual ~Simulator();

    /**
     * @brief Executes wait.
     *
     * @param[in] ms Ms, in milliseconds.
     */
    void wait(int ms);
    /**
     * @brief Sets the button.
     *
     * @param[in] index Zero-based index of the addressed entry.
     * @param[in] pressed Boolean value controlling pressed.
     */
    void setButton(int index, bool pressed);
    /**
     * @brief Sets the encoder.
     *
     * @param[in] pressed Boolean value controlling pressed.
     */
    void setEncoder(bool pressed);
    /**
     * @brief Executes rotate encoder.
     *
     * @param[in] direction Direction used by the operation.
     */
    void rotateEncoder(int direction);
    /**
     * @brief Sets the adc.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] voltage Voltage used by the operation.
     */
    void setAdc(int channel, float voltage);
    /**
     * @brief Sets the dio.
     *
     * @param[in] pin Pin used by the operation.
     * @param[in] state New state or logical level to apply.
     */
    void setDio(int pin, bool state);
    /**
     * @brief Sends a MIDI message through the requested output port.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     */
    void sendMidi(int port, const MidiMessage &message);
    /**
     * @brief Executes reboot.
     */
    void reboot();

    /**
     * @brief Executes screenshot.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     * @param[in] scale Scale used by the operation.
     */
    void screenshot(const std::string &filename, int scale = 1);
    /**
     * @brief Executes screenshot region.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] width Width in pixels or the coordinate domain used by the caller.
     * @param[in] height Height in pixels or the coordinate domain used by the caller.
     * @param[in] scale Scale used by the operation.
     */
    void screenshotRegion(const std::string &filename, int x, int y, int width, int height, int scale = 1);

    /**
     * @brief Returns the target state.
     *
     * @return Reference to the target state.
     */
    const TargetState &targetState() const { return _targetState; }

    /**
     * @brief Returns the ticks.
     *
     * @return Current ticks.
     */
    double ticks();

    typedef std::function<void()> UpdateCallback;
    using UpdateCallbackId = uint64_t;

    /**
     * @brief Adds update callback.
     *
     * @param[in] callback Callback invoked by the operation.
     *
     * @return Stable callback identifier used to unregister the callback later.
     */
    UpdateCallbackId addUpdateCallback(UpdateCallback callback);
    /**
     * @brief Removes update callback.
     *
     * @param[in] id Id used by the operation.
     */
    void removeUpdateCallback(UpdateCallbackId id);

    // Target input/output handling

    /**
     * @brief Registers target tick observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void registerTargetTickObserver(TargetTickHandler *observer);
    /**
     * @brief Registers target input observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void registerTargetInputObserver(TargetInputHandler *observer);
    /**
     * @brief Registers target output observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void registerTargetOutputObserver(TargetOutputHandler *observer);

    /**
     * @brief Unregisters target tick observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void unregisterTargetTickObserver(TargetTickHandler *observer);
    /**
     * @brief Unregisters target input observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void unregisterTargetInputObserver(TargetInputHandler *observer);
    /**
     * @brief Unregisters target output observer.
     *
     * @param[in] observer Observer used by the operation.
     */
    void unregisterTargetOutputObserver(TargetOutputHandler *observer);

    // TargetInputHandler
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based index of the addressed entry.
     * @param[in] pressed Boolean value controlling pressed.
     */
    void writeButton(int index, bool pressed) override;
    /**
     * @brief Writes encoder.
     *
     * @param[in] event Event to process.
     */
    void writeEncoder(EncoderEvent event) override;
    /**
     * @brief Writes adc.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeAdc(int channel, uint16_t value) override;
    /**
     * @brief Writes digital input.
     *
     * @param[in] pin Pin used by the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeDigitalInput(int pin, bool value) override;
    /**
     * @brief Writes midi input.
     *
     * @param[in] event Event to process.
     */
    void writeMidiInput(MidiEvent event) override;

    // TargetOutputHandler
    /**
     * @brief Writes led.
     *
     * @param[in] index Zero-based index of the addressed entry.
     * @param[in] red Boolean value controlling red.
     * @param[in] green Boolean value controlling green.
     */
    void writeLed(int index, bool red, bool green) override;
    /**
     * @brief Writes gate output.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeGateOutput(int channel, bool value) override;
    /**
     * @brief Writes dac.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeDac(int channel, uint16_t value) override;
    /**
     * @brief Writes digital output.
     *
     * @param[in] pin Pin used by the operation.
     * @param[in] value Replacement value to store or apply.
     */
    void writeDigitalOutput(int pin, bool value) override;
    /**
     * @brief Writes lcd.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    void writeLcd(const FrameBuffer &frameBuffer) override;
    /**
     * @brief Writes midi output.
     *
     * @param[in] event Event to process.
     */
    void writeMidiOutput(MidiEvent event) override;

    /**
     * @brief Returns the instance.
     *
     * @return Reference to the instance.
     */
    static Simulator &instance();

private:
    /**
     * @brief Advances the Simulator by one logical step.
     */
    void step();

    Target _target; ///< Lifecycle callbacks for the currently attached simulated target.
    bool _targetCreated = false; ///< Whether target created is true in the current state.

    uint32_t _tick = 0; ///< Current absolute sequencer engine tick.

    std::vector<TargetTickHandler *> _targetTickObservers; ///< Pointer to target tick observers; `nullptr` denotes that no object/resource is assigned.
    std::vector<TargetInputHandler *> _targetInputObservers; ///< Pointer to target input observers; `nullptr` denotes that no object/resource is assigned.
    std::vector<TargetOutputHandler *> _targetOutputObservers; ///< Pointer to target output observers; `nullptr` denotes that no object/resource is assigned.

    /**
     * @brief Provides update callback entry behavior for the desktop simulator.
     */
    struct UpdateCallbackEntry {
        UpdateCallbackId id; ///< Id state owned by this component.
        UpdateCallback callback; ///< Callback invoked for callback events.
    };

    std::vector<UpdateCallbackEntry> _updateCallbacks; ///< Callback invoked for updates events.
    UpdateCallbackId _nextUpdateCallbackId = 1; ///< Next update callback id scheduled or expected by this component.

    TargetState _targetState; ///< Current simulated hardware target state.
    TargetStateTracker _targetStateTracker; ///< Tracker that records changes to the simulated target state.
};

} // namespace sim
