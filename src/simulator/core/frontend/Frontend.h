/**
 * @file Frontend.h
 * @brief Declares the Frontend component used by the simulator frontend.
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

#include "Common.h"
#include "Window.h"
#include "Audio.h"
#include "InstrumentSetup.h"
#include "Midi.h"
#include "ClockSource.h"

#include "widgets/Button.h"
#include "widgets/Display.h"
#include "widgets/Encoder.h"
#include "widgets/Label.h"
#include "widgets/Led.h"
#include "widgets/Rotary.h"
#include "widgets/Jack.h"

#include "core/Simulator.h"

#include <string>
#include <vector>

#include <cstdint>

namespace sim {

/**
 * @brief Provides frontend behavior for the desktop simulator.
 */
class Frontend : private TargetInputHandler, TargetOutputHandler {
public:
    /**
     * @brief Constructs a Frontend instance.
     *
     * @param[in] simulator Simulator instance associated with the frontend/platform object.
     */
    Frontend(Simulator &simulator);
    /**
     * @brief Destroys the Frontend instance.
     */
    ~Frontend();

    /**
     * @brief Returns main.
     *
     * @param[in] argc Number of command-line arguments, including the executable name.
     * @param[in] argv Array of null-terminated command-line argument strings.
     *
     * @return Result of main().
     */
    int main(int argc, char *argv[]);

    /**
     * @brief Runs the simulator/application main loop.
     */
    void run();

    /**
     * @brief Closes the active page, file, dialog, or resource.
     */
    void close();

private:
    /**
     * @brief Returns the terminate.
     *
     * @return `true` if terminate; otherwise `false`.
     */
    bool terminate() const;
    /**
     * @brief Advances the Frontend by one logical step.
     */
    void step();
    /**
     * @brief Updates the Frontend for the current service cycle.
     */
    void update();
    /**
     * @brief Renders this component into the supplied target.
     */
    void render();
    /**
     * @brief Delays execution for the requested duration.
     *
     * @param[in] ms Ms, in milliseconds.
     */
    void delay(int ms);

    /**
     * @brief Returns the ticks.
     *
     * @return Current platform/simulator tick counter.
     */
    double ticks() const;

    /**
     * @brief Sets the up.
     */
    void setup();
    /**
     * @brief Sets the up window.
     */
    void setupWindow();
    /**
     * @brief Sets the up frontpanel.
     */
    void setupFrontpanel();
    /**
     * @brief Sets the up controls.
     */
    void setupControls();

    /**
     * @brief Sets the up midi.
     */
    void setupMidi();
    /**
     * @brief Sets the up instruments.
     */
    void setupInstruments();

    // TargetInputHandler
    /**
     * @brief Writes button.
     *
     * @param[in] index Zero-based write button index.
     * @param[in] pressed Whether pressed is enabled for this operation.
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
     * @param[in] value Value to write adc.
     */
    void writeAdc(int channel, uint16_t value) override;
    /**
     * @brief Writes digital input.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital input.
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
     * @param[in] index Zero-based write led index.
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     */
    void writeLed(int index, bool red, bool green) override;
    /**
     * @brief Writes gate output.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write gate output.
     */
    void writeGateOutput(int channel, bool value) override;
    /**
     * @brief Writes dac.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Value to write dac.
     */
    void writeDac(int channel, uint16_t value) override;
    /**
     * @brief Writes digital output.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] value Value to write digital output.
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
     * @brief Reference to simulator owned by another component.
     */
    Simulator &_simulator; ///< Reference to simulator owned by another component.
    Audio _audio; ///< Audio subsystem owned by the simulator frontend.
    std::unique_ptr<InstrumentSetup> _instruments; ///< Owned setup containing the simulator audio instruments.

    /**
     * @brief Simulator value representing timer frequency.
     */
    double _timerFrequency; ///< Simulator value representing timer frequency.
    /**
     * @brief Simulator value representing timer start.
     */
    double _timerStart; ///< Simulator value representing timer start.

    /**
     * @brief Most recently observed update ticks.
     */
    uint32_t _lastUpdateTicks = 0; ///< Simulator tick of the previous frontend update.
    /**
     * @brief Most recently observed render ticks.
     */
    double _lastRenderTicks = 0.0; ///< Host-time timestamp of the previous rendered frame.

    Midi _midi; ///< MIDI driver/transport used by this component.
    std::shared_ptr<Midi::Port> _midiPort; ///< Shared host-MIDI port connected to the simulated DIN MIDI interface.
    std::shared_ptr<Midi::Port> _usbMidiPort; ///< Shared host-MIDI port connected to the simulated USB-MIDI interface.

    std::unique_ptr<ClockSource> _clockSource; ///< Owned frontend clock source that drives simulated external clock input.

    Window::Ptr _window; ///< Simulator window that owns and renders the front-panel widgets.
    Encoder::Ptr _encoder; ///< Front-panel encoder widget connected to the simulated encoder driver.
    Display::Ptr _lcd; ///< Front-panel display widget showing the simulated LCD framebuffer.
    Jack::Ptr _midiInputJack; ///< Front-panel jack widget representing DIN MIDI input.
    Jack::Ptr _midiOutputJack; ///< Front-panel jack widget representing DIN MIDI output.
    std::vector<Jack::Ptr> _digitalInputJacks; ///< Front-panel jack widgets mapped to simulated digital inputs.
    std::vector<Jack::Ptr> _digitalOutputJacks; ///< Front-panel jack widgets mapped to simulated digital outputs.
    std::vector<Jack::Ptr> _cvInputJacks; ///< Front-panel jack widgets mapped to simulated CV inputs.
    std::vector<Jack::Ptr> _gateOutputJacks; ///< Front-panel jack widgets mapped to simulated gate outputs.
    std::vector<Jack::Ptr> _cvOutputJacks; ///< Front-panel jack widgets mapped to simulated CV outputs.
    std::vector<Button::Ptr> _buttons; ///< Front-panel button widgets mapped to the simulated button matrix.
    std::vector<Led::Ptr> _leds; ///< Front-panel LED widgets mapped to the simulated LED matrix.
    std::vector<Label::Ptr> _labels; ///< Static text labels rendered on the simulated front panel.

    // Frontend-only display timers for digital input jacks to improve visibility
    std::vector<double> _digitalInputDisplayUntil; ///< Per-input host-time deadline until which a digital-input pulse remains highlighted in the UI.

    // Frontend-only scheduled falling edges for simulated digital inputs
    // When the simulator issues a short pulse we schedule the falling edge
    // here (ms ticks) so the engine receives a measurable pulse width.
    std::vector<double> _digitalInputPendingFalseAt; ///< Per-input host-time deadline for applying a deferred simulated falling edge.


#ifdef __EMSCRIPTEN__
    /**
     * @brief Runs one iteration of the Emscripten/browser main loop.
     */
    friend void emscriptenMainLoop();
#endif
};

} // namespace sim
