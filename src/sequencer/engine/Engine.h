/**
 * @file Engine.h
 * @brief Declares the Engine component used by the sequencer engine.
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

#include "EngineState.h"
#include "Clock.h"
#include "TapTempo.h"
#include "NudgeTempo.h"
#include "TrackEngine.h"
#include "NoteTrackEngine.h"
#include "CurveTrackEngine.h"
#include "MidiCvTrackEngine.h"
#include "LfoTrackEngine.h"
#include "CvInput.h"
#include "CvOutput.h"
#include "RoutingEngine.h"
#include "MidiOutputEngine.h"
#include "MidiPort.h"
#include "MidiLearn.h"
#include "CvGateToMidiConverter.h"
#include "UpdateReducer.h"

#include "model/Model.h"

#include "drivers/ClockTimer.h"
#include "drivers/Adc.h"
#include "drivers/Dac.h"
#include "drivers/Dio.h"
#include "drivers/GateOutput.h"
#include "drivers/Midi.h"
#include "drivers/UsbMidi.h"

#include <array>

#include <cstdint>

/**
 * @brief Implements engine runtime processing.
 */
class Engine : private Clock::Listener {
public:
    typedef Container<NoteTrackEngine, CurveTrackEngine, MidiCvTrackEngine, LfoTrackEngine> TrackEngineContainer;
    typedef std::array<TrackEngineContainer, CONFIG_TRACK_COUNT> TrackEngineContainerArray;
    typedef std::array<TrackEngine *, CONFIG_TRACK_COUNT> TrackEngineArray;
    typedef std::array<UpdateReducer<os::time::ms(25)>, CONFIG_TRACK_COUNT> TrackUpdateReducerArray;

    typedef std::function<bool(MidiPort port, uint8_t cable, const MidiMessage &message)> MidiReceiveHandler;

    typedef std::function<void(uint16_t vendorId, uint16_t productId)> UsbMidiConnectHandler;
    typedef std::function<void()> UsbMidiDisconnectHandler;

    typedef std::function<void(const char *text, uint32_t duration)> MessageHandler;

    /**
     * @brief Enumerates the supported clock source values.
     */
    enum ClockSource {
        ClockSourceExternal, ///< Selects the clock source external clock source.
        ClockSourceMidi, ///< Selects the clock source midi clock source.
        ClockSourceUsbMidi, ///< Selects the clock source usb midi clock source.
    };

    /**
     * @brief Stores runtime counters and timing statistics reported by the enclosing component.
     */
    struct Stats {
        /**
         * @brief Runtime value representing uptime.
         */
        uint32_t uptime; ///< Accumulated application uptime in system ticks/time units.
        /**
         * @brief Runtime value representing midi rx overflow.
         */
        uint32_t midiRxOverflow; ///< Count of dropped/overflowed MIDI receive events.
        /**
         * @brief Runtime value representing usb midi rx overflow.
         */
        uint32_t usbMidiRxOverflow; ///< Count of dropped/overflowed USB-MIDI receive events.
    };

    /**
     * @brief Constructs a Engine instance.
     *
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] clockTimer Clock timer that drives sequencer tick generation.
     * @param[in] adc ADC driver used to sample control-voltage inputs.
     * @param[in] dac DAC driver used to emit control-voltage values.
     * @param[in] dio Digital-I/O driver used for external clock, reset and panel signals.
     * @param[in] gateOutput Gate-output driver receiving sequencer gate states.
     * @param[in] midi DIN-MIDI transport used by the sequencer engine.
     * @param[in] usbMidi USB-MIDI transport used by the sequencer engine.
     */
    Engine(Model &model, ClockTimer &clockTimer, Adc &adc, Dac &dac, Dio &dio, GateOutput &gateOutput, Midi &midi, UsbMidi &usbMidi);

    /**
     * @brief Initializes the Engine and its runtime resources.
     */
    void init();
    /**
     * @brief Updates the Engine for the current service cycle.
     */
    void update();

    // locking temporarily puts the engine in a state where completely skips all updates
    // lock should only be hold for very short amounts of time
    /**
     * @brief Prevents normal engine updates during a short critical operation.
     */
    void lock();
    /**
     * @brief Re-enables normal engine updates after a lock.
     */
    void unlock();
    /**
     * @brief Reports whether locked.
     *
     * @return `true` if locked; otherwise `false`.
     */
    bool isLocked() const { return _locked; }

    // suspending temporarily puts the engine in a state where it only processes basic events but skips all updates
    // suspending can be used during longer periods of time (e.g. file operations)
    /**
     * @brief Suspends nonessential engine processing for a longer operation.
     */
    void suspend();
    /**
     * @brief Resumes normal engine processing after suspension.
     */
    void resume();
    /**
     * @brief Reports whether suspended.
     *
     * @return `true` if suspended; otherwise `false`.
     */
    bool isSuspended() const { return _suspended; }

    // clock control
    /**
     * @brief Toggles play.
     *
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void togglePlay(bool shift = false);
    /**
     * @brief Starts sequencer transport through the Clock component.
     */
    void clockStart();
    /**
     * @brief Stops sequencer transport through the Clock component.
     */
    void clockStop();
    /**
     * @brief Continues sequencer transport without resetting position.
     */
    void clockContinue();
    /**
     * @brief Resets sequencer transport position.
     */
    void clockReset();
    /**
     * @brief Returns the clock running.
     *
     * @return `true` if clock running; otherwise `false`.
     */
    bool clockRunning() const;

    // recording
    /**
     * @brief Toggles recording.
     */
    void toggleRecording();
    /**
     * @brief Sets the recording.
     *
     * @param[in] recording `true` to enable/set recording; `false` to disable/clear it.
     */
    void setRecording(bool recording);
    /**
     * @brief Returns the recording.
     *
     * @return `true` if recording; otherwise `false`.
     */
    bool recording() const;

    // tempo
    /**
     * @brief Returns the tempo.
     *
     * @return Project tempo in beats per minute.
     */
    float tempo() const { return _clock.bpm(); }

    // tap tempo
    /**
     * @brief Clears tap-tempo history.
     */
    void tapTempoReset();
    /**
     * @brief Adds one tap and updates the derived tempo.
     */
    void tapTempoTap();

    // nudge tempo
    /**
     * @brief Sets the direction of temporary tempo nudging.
     *
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     */
    void nudgeTempoSetDirection(int direction);
    /**
     * @brief Returns the nudge tempo strength.
     *
     * @return Current temporary tempo-nudge strength.
     */
    float nudgeTempoStrength() const;

    // time base
    /**
     * @brief Returns the current absolute sequencer engine tick.
     *
     * @return Current absolute sequencer engine tick.
     */
    uint32_t tick() const { return _tick; }
    /**
     * @brief Returns the note divisor.
     *
     * @return Sequencer tick divisor represented by one beat of the configured time signature.
     */
    uint32_t noteDivisor() const;
    /**
     * @brief Returns the measure divisor.
     *
     * @return Sequencer tick divisor represented by one complete measure.
     */
    uint32_t measureDivisor() const;
    /**
     * @brief Returns the measure fraction.
     *
     * @return Fractional position within the current measure.
     */
    float measureFraction() const;
    /**
     * @brief Synchronizes divisor.
     *
     * @return Current synchronization divisor in engine ticks.
     */
    uint32_t syncDivisor() const;
    /**
     * @brief Synchronizes fraction.
     *
     * @return Fractional position within the current synchronization interval.
     */
    float syncFraction() const;

    /**
     * @brief Returns the cv input.
     *
     * @return Reference to the cv input.
     */
    const CvInput &cvInput() const { return _cvInput; }
    /**
     * @brief Returns the current CV value for a track output channel.
     *
     * @return Reference to the cv output.
     */
    const CvOutput &cvOutput() const { return _cvOutput; }
    /**
     * @brief Returns the current gate state for a track output channel.
     *
     * @return Current logical gate level for the addressed output channel.
     */
    const uint8_t gateOutput() const { return _gateOutput.gates(); }

    // gate overrides
    /**
     * @brief Returns the gate output override.
     *
     * @return `true` if gate output override; otherwise `false`.
     */
    bool gateOutputOverride() const { return _gateOutputOverride; }
    /**
     * @brief Sets the gate output override.
     *
     * @param[in] enabled `true` to enable the behavior; `false` to disable it.
     */
    void setGateOutputOverride(bool enabled) { _gateOutputOverride = enabled; }
    /**
     * @brief Sets the gate output.
     *
     * @param[in] gates Gate bit mask or gate-state collection consumed by the operation.
     */
    void setGateOutput(uint8_t gates) { _gateOutputOverrideValue = gates; }

    // cv overrides
    /**
     * @brief Returns the cv output override.
     *
     * @return `true` if cv output override; otherwise `false`.
     */
    bool cvOutputOverride() const { return _cvOutputOverride; }
    /**
     * @brief Sets the cv output override.
     *
     * @param[in] enabled `true` to enable the behavior; `false` to disable it.
     */
    void setCvOutputOverride(bool enabled) { _cvOutputOverride = enabled; }
    /**
     * @brief Sets the cv output.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value New cv output to store or apply.
     */
    void setCvOutput(int channel, float value) { _cvOutputOverrideValues[channel] = value; }

    /**
     * @brief Returns the clock.
     *
     * @return Reference to the clock.
     */
    const Clock &clock() const { return _clock; }
          /**
           * @brief Returns the clock.
           *
           * @return Reference to the clock.
           */
          Clock &clock()       { return _clock; }

    /**
     * @brief Returns the state.
     *
     * @return Reference to the state.
     */
    const EngineState &state() const { return _state; }

    /**
     * @brief Returns the track engines.
     *
     * @return Reference to the track engines.
     */
    const TrackEngineArray &trackEngines() const { return _trackEngines; }
          /**
           * @brief Returns the track engines.
           *
           * @return Reference to the track engines.
           */
          TrackEngineArray &trackEngines()       { return _trackEngines; }

    /**
     * @brief Returns track engine.
     *
     * @param[in] index Zero-based track engine index.
     *
     * @return Reference to the track engine.
     */
    const TrackEngine &trackEngine(int index) const { return *_trackEngines[index]; }
          /**
           * @brief Returns track engine.
           *
           * @param[in] index Zero-based track engine index.
           *
           * @return Reference to the track engine.
           */
          TrackEngine &trackEngine(int index)       { return *_trackEngines[index]; }

    /**
     * @brief Selects ed track engine.
     *
     * @return Reference to the selected track engine.
     */
    const TrackEngine &selectedTrackEngine() const { return *_trackEngines[_model.project().selectedTrackIndex()]; }
          /**
           * @brief Selects ed track engine.
           *
           * @return Reference to the selected track engine.
           */
          TrackEngine &selectedTrackEngine()       { return *_trackEngines[_model.project().selectedTrackIndex()]; }

    /**
     * @brief Returns the routing engine.
     *
     * @return Reference to the routing engine.
     */
    const RoutingEngine &routingEngine() const { return _routingEngine; }
          /**
           * @brief Returns the routing engine.
           *
           * @return Reference to the routing engine.
           */
          RoutingEngine &routingEngine()       { return _routingEngine; }

    /**
     * @brief Returns the midi output engine.
     *
     * @return Reference to the midi output engine.
     */
    const MidiOutputEngine &midiOutputEngine() const { return _midiOutputEngine; }
          /**
           * @brief Returns the midi output engine.
           *
           * @return Reference to the midi output engine.
           */
          MidiOutputEngine &midiOutputEngine()       { return _midiOutputEngine; }

    /**
     * @brief Returns the midi learn.
     *
     * @return Reference to the midi learn.
     */
    const MidiLearn &midiLearn() const { return _midiLearn; }
          /**
           * @brief Returns the midi learn.
           *
           * @return Reference to the midi learn.
           */
          MidiLearn &midiLearn()       { return _midiLearn; }

    /**
     * @brief Returns the track engines consistent.
     *
     * @return `true` if track engines consistent; otherwise `false`.
     */
    bool trackEnginesConsistent() const;

    /**
     * @brief Sends a MIDI message through the requested output port.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if send midi; otherwise `false`.
     */
    bool sendMidi(MidiPort port, uint8_t cable, const MidiMessage &message);
    /**
     * @brief Sets the midi receive handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setMidiReceiveHandler(MidiReceiveHandler handler) { _midiReceiveHandler = handler; }
    /**
     * @brief Sets the usb midi connect handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setUsbMidiConnectHandler(UsbMidiConnectHandler handler) { _usbMidiConnectHandler = handler; }
    /**
     * @brief Sets the usb midi disconnect handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setUsbMidiDisconnectHandler(UsbMidiDisconnectHandler handler) { _usbMidiDisconnectHandler = handler; }

    // message handling
    /**
     * @brief Displays a transient user message.
     *
     * @param[in] text Null-terminated text string consumed or displayed by the operation.
     * @param[in] duration Duration in the unit defined by the owning API.
     */
    void showMessage(const char *text, uint32_t duration = 1000);
    /**
     * @brief Sets the message handler.
     *
     * @param[in] handler Callback invoked when the associated event occurs.
     */
    void setMessageHandler(MessageHandler handler);

    /**
     * @brief Returns the stats.
     *
     * @return Latest sequencer engine runtime statistics.
     */
    Stats stats() const;

private:
    // Clock::Listener
    /**
     * @brief Handles the clock output callback.
     *
     * @param[in] state New state or logical level to apply.
     */
    virtual void onClockOutput(const Clock::OutputState &state) override;
    /**
     * @brief Handles the clock midi callback.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     */
    virtual void onClockMidi(uint8_t data) override;

    /**
     * @brief Rebuilds/updates track-engine setup from the current model.
     */
    void updateTrackSetups();
    /**
     * @brief Publishes track-engine CV/gate outputs to hardware/platform drivers.
     */
    void updateTrackOutputs();
    /**
     * @brief Resets the Engine to its initial runtime state.
     */
    void reset();
    /**
     * @brief Applies pending play-state transitions at the current tick.
     *
     * @param[in] ticked Whether ticked is enabled for this operation.
     */
    void updatePlayState(bool ticked);
    /**
     * @brief Applies manual CV/gate output overrides.
     */
    void updateOverrides();

    /**
     * @brief Handles connection of a USB-MIDI device.
     *
     * @param[in] vendorId USB vendor identifier (VID).
     * @param[in] productId USB product identifier (PID).
     */
    void usbMidiConnect(uint16_t vendorId, uint16_t productId);
    /**
     * @brief Handles disconnection of a USB-MIDI device.
     */
    void usbMidiDisconnect();

    /**
     * @brief Processes MIDI input received by the engine.
     */
    void receiveMidi();
    /**
     * @brief Processes MIDI input received by the engine.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    void receiveMidi(MidiPort port, uint8_t cable, const MidiMessage &message);
    /**
     * @brief Processes a MIDI message for live monitoring and recording.
     *
     * @param[in] message Message to process.
     */
    void monitorMidi(const MidiMessage &message);

    /**
     * @brief Initializes clock.
     */
    void initClock();
    /**
     * @brief Applies project clock settings to the runtime Clock component.
     */
    void updateClockSetup();

    /**
     * @brief Persistent sequencer model used by this component.
     */
    Model &_model; ///< Persistent sequencer model used by this component.
    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
    /**
     * @brief Reference to dio owned by another component.
     */
    Dio &_dio; ///< Reference to dio owned by another component.
    /**
     * @brief Current digital gate output state.
     */
    GateOutput &_gateOutput; ///< Current digital gate output state.
    /**
     * @brief Reference to midi owned by another component.
     */
    Midi &_midi; ///< Reference to midi owned by another component.
    /**
     * @brief Reference to usb midi owned by another component.
     */
    UsbMidi &_usbMidi; ///< Reference to usb midi owned by another component.

    EngineState _state; ///< Transport/recording state owned by the sequencer engine.

    CvInput _cvInput; ///< CV-input processing subsystem owned by the sequencer engine.
    /**
     * @brief Current control-voltage output value in the engine output domain.
     */
    CvOutput _cvOutput; ///< Current control-voltage output value in the engine output domain.

    Clock _clock; ///< Master/slave clock subsystem owned by the sequencer engine.
    TapTempo _tapTempo; ///< Tap-tempo estimator owned by the sequencer engine.
    NudgeTempo _nudgeTempo; ///< Temporary tempo-nudge controller owned by the sequencer engine.

    TrackEngineContainerArray _trackEngineContainers; ///< In-place storage owning the concrete runtime engine for each sequencer track.
    TrackEngineArray _trackEngines; ///< Non-owning pointers to the active runtime track engines, indexed by track.
    TrackUpdateReducerArray _trackUpdateReducers; ///< Per-track update-rate reducers used to distribute expensive track processing.

    MidiOutputEngine _midiOutputEngine; ///< MIDI-output rendering subsystem owned by the sequencer engine.

    RoutingEngine _routingEngine; ///< Runtime modulation/routing subsystem owned by the sequencer engine.
    MidiLearn _midiLearn; ///< MIDI-learn state machine owned by the sequencer engine.
    MidiReceiveHandler _midiReceiveHandler; ///< Callback invoked when the engine receives a decoded MIDI message.
    UsbMidiConnectHandler _usbMidiConnectHandler; ///< Callback invoked when a USB-MIDI device becomes available.
    UsbMidiDisconnectHandler _usbMidiDisconnectHandler; ///< Callback invoked when a USB-MIDI device disconnects.

    CvGateToMidiConverter _cvGateToMidiConverter; ///< Converter that translates incoming CV/gate activity into MIDI events.

    // locking
    /**
     * @brief Runtime value representing request lock.
     */
    volatile uint32_t _requestLock = 0; ///< Outstanding engine-lock request count/flag.
    /**
     * @brief Runtime value representing locked.
     */
    volatile uint32_t _locked = 0; ///< Current engine lock count/state.

    // suspending
    /**
     * @brief Runtime value representing request suspend.
     */
    volatile uint32_t _requestSuspend = 0; ///< Outstanding engine-suspend request count/flag.
    /**
     * @brief Runtime value representing suspended.
     */
    volatile uint32_t _suspended = 0; ///< Current engine suspend count/state.

    /**
     * @brief Current absolute sequencer engine tick.
     */
    uint32_t _tick = 0; ///< Current absolute sequencer engine tick.

    uint32_t _lastSystemTicks = 0; ///< Previous platform system tick sampled by `update()`, used to derive elapsed update time.

    // midi monitoring
    /**
     * @brief Most recently observed midi input mode.
     */
    struct {
        Types::MidiInputMode lastMidiInputMode;
        MidiSourceConfig lastMidiInputSource;
        Types::CvGateInput lastCvGateInput;

        bool inputChanged(const Project &project) {
            bool changed =
                project.midiInputMode() != lastMidiInputMode ||
                project.midiInputSource() != lastMidiInputSource ||
                project.cvGateInput() != lastCvGateInput;
            if (changed) {
                lastMidiInputMode = project.midiInputMode();
                lastMidiInputSource = project.midiInputSource();
                lastCvGateInput = project.cvGateInput();
            }
            return changed;
        }

        int8_t lastNote = -1;
        int8_t lastTrack = -1;
    } _midiMonitoring;

    // gate output overrides
    /**
     * @brief Whether gate output override is true in the current state.
     */
    bool _gateOutputOverride = false; ///< True while manual/test code overrides the normal gate-output engine values.
    /**
     * @brief Runtime value representing gate output override value.
     */
    uint8_t _gateOutputOverrideValue = 0; ///< Gate bit mask forced while gate-output override is enabled.

    // cv output overrides
    /**
     * @brief Whether cv output override is true in the current state.
     */
    bool _cvOutputOverride = false; ///< True while manual/test code overrides the normal CV-output engine values.
    /**
     * @brief Fixed-capacity storage for cv output override values.
     */
    std::array<float, CvOutput::Channels> _cvOutputOverrideValues; ///< Per-channel CV values emitted while manual output override is active.

    MessageHandler _messageHandler; ///< Callback used by the engine to publish user-visible status messages.

};
