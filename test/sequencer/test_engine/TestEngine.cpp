/**
 * @file TestEngine.cpp
 * @brief Implements the TestEngine component used by the unit test.
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
#include <unity.h>

#include "SequencerApp.h"
#include "model/NoteSequence.h"

#include "core/Simulator.h"
#include "core/MidiEvent.h"

#include <memory>

namespace {

class SequencerHarness {
public:
    SequencerHarness() :
        _simulator(makeTarget())
    {
        _simulator.reboot();
    }

    SequencerApp &app() { return *_app; }

    void sendDinMidi(const MidiMessage &message) {
        _simulator.sendMidi(0, message);
    }

    void sendUsbMidi(const MidiMessage &message) {
        _simulator.sendMidi(1, message);
    }

    void connectUsbMidi(uint16_t vendorId, uint16_t productId) {
        _simulator.writeMidiInput(sim::MidiEvent::makeConnect(1, vendorId, productId));
    }

    void disconnectUsbMidi() {
        _simulator.writeMidiInput(sim::MidiEvent::makeDisconnect(1));
    }

    void waitMs(int ms) {
        _simulator.wait(ms);
    }

    void setResetInput(bool high) {
        _simulator.setDio(1, high);
    }

    void setClockInput(bool high) {
        _simulator.setDio(0, high);
    }

private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = [this] () {
            _app.reset(new SequencerApp());
        };
        target.destroy = [this] () {
            _app.reset();
        };
        target.update = [this] () {
            _app->update();
        };
        return target;
    }

    std::unique_ptr<SequencerApp> _app;
    sim::Simulator _simulator;
};

static uint32_t sequenceDivisorTicks(const NoteSequence &sequence) {
    return sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
}

static void forceMasterClockMode(SequencerApp &app) {
    auto &clockSetup = app.model.project().clockSetup();
    clockSetup.setMode(ClockSetup::Mode::Master);
    app.engine.update();
}

static uint32_t gateOnTick(const NoteSequence &sequence, int gateOffset) {
    return (sequenceDivisorTicks(sequence) * gateOffset) / (NoteSequence::GateOffset::Max + 1);
}

static uint32_t gateLengthTicks(const NoteSequence &sequence, int gateLength) {
    return (sequenceDivisorTicks(sequence) * (gateLength + 1)) / NoteSequence::Length::Range;
}

// Creates a deterministic single-step note pattern for timing-related tests.
static NoteTrackEngine &prepareSingleStepEngine(
    SequencerApp &app,
    int gateOffset,
    int gateLength = NoteSequence::Length::Max,
    bool gateActive = true)
{
    auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
    sequence.clearSteps();
    sequence.setFirstStep(0);
    sequence.setLastStep(0);

    auto &step = sequence.step(0);
    step.setGate(gateActive);
    step.setGateProbability(NoteSequence::GateProbability::Max);
    step.setLength(gateLength);
    step.setLengthVariationRange(0);
    step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
    step.setRetrigger(0);
    step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
    step.setGateOffset(gateOffset);

    auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();
    engine.reset();
    return engine;
}

// Creates a two-step pattern to validate that each step keeps its own offset timing.
static NoteTrackEngine &prepareTwoStepEngine(
    SequencerApp &app,
    int step0Offset,
    int step1Offset,
    int gateLength = 3)
{
    auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
    sequence.clearSteps();
    sequence.setFirstStep(0);
    sequence.setLastStep(1);

    for (int index = 0; index < 2; ++index) {
        auto &step = sequence.step(index);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(gateLength);
        step.setLengthVariationRange(0);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setGateOffset(index == 0 ? step0Offset : step1Offset);
    }

    auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();
    engine.reset();
    return engine;
}

} // namespace

void test_01_track_engines_stay_consistent_after_track_mode_changes() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;

        TEST_ASSERT_TRUE(engine.trackEnginesConsistent());

        project.setTrackMode(0, Track::TrackMode::Curve);
        engine.update();
        TEST_ASSERT_TRUE(engine.trackEnginesConsistent());
        TEST_ASSERT_TRUE((engine.trackEngine(0).trackMode()) == (Track::TrackMode::Curve));

        project.setTrackMode(0, Track::TrackMode::Note);
        engine.update();
        TEST_ASSERT_TRUE(engine.trackEnginesConsistent());
        TEST_ASSERT_TRUE((engine.trackEngine(0).trackMode()) == (Track::TrackMode::Note));
    
}

void test_02_track_mode_midicv_is_instantiated_and_can_switch_back_to_note() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;

        project.setTrackMode(0, Track::TrackMode::MidiCv);
        engine.update();
        TEST_ASSERT_TRUE((engine.trackEngine(0).trackMode()) == (Track::TrackMode::MidiCv));

        project.setTrackMode(0, Track::TrackMode::Note);
        engine.update();
        TEST_ASSERT_TRUE((engine.trackEngine(0).trackMode()) == (Track::TrackMode::Note));
    
}

void test_03_trackenginesconsistent_reports_mismatch_before_setup_update() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;

        TEST_ASSERT_TRUE(engine.trackEnginesConsistent());

        project.setTrackMode(0, Track::TrackMode::Curve);
        TEST_ASSERT_FALSE(engine.trackEnginesConsistent());

        engine.update();
        TEST_ASSERT_TRUE(engine.trackEnginesConsistent());
    
}

void test_04_immediate_pattern_request_updates_active_note_sequence() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();

        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();
        TEST_ASSERT_TRUE((playState.trackState(0).pattern()) == (0));
        TEST_ASSERT_TRUE(&noteEngine.sequence() == &project.track(0).noteTrack().sequence(0));

        playState.selectTrackPattern(0, 1, PlayState::Immediate);
        engine.update();

        TEST_ASSERT_TRUE((playState.trackState(0).pattern()) == (1));
        TEST_ASSERT_TRUE(&noteEngine.sequence() == &project.track(0).noteTrack().sequence(1));
    
}

void test_05_latched_mute_request_applies_only_after_commit() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();

        TEST_ASSERT_FALSE(playState.trackState(0).mute());

        playState.muteTrack(0, PlayState::Latched);
        engine.update();
        TEST_ASSERT_FALSE(playState.trackState(0).mute());

        playState.commitLatchedRequests();
        engine.update();
        TEST_ASSERT_TRUE(playState.trackState(0).mute());
    
}

void test_06_immediate_playsong_with_invalid_slot_is_ignored_and_request_gets_cleared() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();
        auto &songState = playState.songState();

        // Prepare one valid slot and request an invalid slot index.
        project.song().clear();
        project.song().insertSlot(0);

        playState.playSong(99, PlayState::Immediate);
        engine.update();

        TEST_ASSERT_FALSE(songState.playing());
        TEST_ASSERT_FALSE(songState.hasPlayRequests());
    
}

void test_07_latched_playsong_applies_only_after_latched_requests_are_committed() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();
        auto &songState = playState.songState();

        project.song().clear();
        project.song().insertSlot(0);
        project.song().setPattern(0, 0, 3);

        playState.playSong(0, PlayState::Latched);
        engine.update();
        TEST_ASSERT_FALSE(songState.playing());
        TEST_ASSERT_TRUE((playState.trackState(0).pattern()) == (0));

        playState.commitLatchedRequests();
        engine.update();
        TEST_ASSERT_TRUE(songState.playing());
        TEST_ASSERT_TRUE((songState.currentSlot()) == (0));
        TEST_ASSERT_TRUE((songState.currentRepeat()) == (0));
        TEST_ASSERT_TRUE((playState.trackState(0).pattern()) == (3));
        TEST_ASSERT_FALSE(songState.hasPlayRequests());
    
}

void test_08_pattern_change_or_stopsong_request_terminates_song_playback() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();
        auto &songState = playState.songState();

        project.song().clear();
        project.song().insertSlot(0);
        project.song().setPattern(0, 0, 2);

        playState.playSong(0, PlayState::Immediate);
        engine.update();
        TEST_ASSERT_TRUE(songState.playing());

        playState.selectTrackPattern(0, 1, PlayState::Immediate);
        engine.update();
        TEST_ASSERT_FALSE(songState.playing());
        TEST_ASSERT_TRUE((playState.trackState(0).pattern()) == (1));

        playState.playSong(0, PlayState::Immediate);
        engine.update();
        TEST_ASSERT_TRUE(songState.playing());

        playState.stopSong(PlayState::Immediate);
        engine.update();
        TEST_ASSERT_FALSE(songState.playing());
    
}

void test_09_playsong_applies_slot_mutes_when_track_has_mutes_in_song() {
        SequencerHarness harness;
        auto &project = harness.app().model.project();
        auto &engine = harness.app().engine;
        auto &playState = project.playState();

        project.song().clear();
        project.song().insertSlot(0);
        project.song().setMute(0, 0, true);

        playState.playSong(0, PlayState::Immediate);
        engine.update();

        TEST_ASSERT_TRUE(playState.trackState(0).mute());
    
}

void test_10_clock_setup_dirty_flag_is_cleared_by_engine_update() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.clearDirty();
        TEST_ASSERT_FALSE(clockSetup.isDirty());

        clockSetup.setClockOutputPulse(clockSetup.clockOutputPulse() + 1);
        TEST_ASSERT_TRUE(clockSetup.isDirty());

        engine.update();
        TEST_ASSERT_FALSE(clockSetup.isDirty());
    
}

void test_11_clock_input_divisor_accepts_common_external_ppqn_values() {
        SequencerHarness harness;
        auto &clockSetup = harness.app().model.project().clockSetup();

        const int validPpqnValues[] = { 3, 12, 16, 24 };
        for (int ppqn : validPpqnValues) {
            clockSetup.clearDirty();
            clockSetup.setClockInputDivisor(ppqn);
            TEST_ASSERT_TRUE((clockSetup.clockInputDivisor()) == (ppqn));
            TEST_ASSERT_TRUE(clockSetup.isDirty());
        }
    
}

void test_12_clock_input_divisor_clamping_and_dirty_flag_consumption_are_stable() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.clearDirty();
        clockSetup.setClockInputDivisor(0);
        TEST_ASSERT_TRUE((clockSetup.clockInputDivisor()) == (1));
        TEST_ASSERT_TRUE(clockSetup.isDirty());

        // Engine::updateClockSetup consumes pending setup changes.
        engine.update();
        TEST_ASSERT_FALSE(clockSetup.isDirty());

        clockSetup.setClockInputDivisor(999);
        TEST_ASSERT_TRUE((clockSetup.clockInputDivisor()) == (192));
        TEST_ASSERT_TRUE(clockSetup.isDirty());

        engine.update();
        TEST_ASSERT_FALSE(clockSetup.isDirty());
    
}

void test_13_sendmidi_rejects_cvgate_port() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        bool sent = engine.sendMidi(MidiPort::CvGate, 0, MidiMessage::makeNoteOn(0, 60, 100));
        TEST_ASSERT_FALSE(sent);
    
}

void test_14_sendmidi_accepts_midi_and_usb_midi_output_ports() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        const auto message = MidiMessage::makeNoteOn(0, 60, 100);

        TEST_ASSERT_TRUE(engine.sendMidi(MidiPort::Midi, 0, message));
        TEST_ASSERT_TRUE(engine.sendMidi(MidiPort::UsbMidi, 0, message));
    
}

void test_15_message_handler_receives_text_and_duration() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        bool called = false;
        const char *receivedText = nullptr;
        uint32_t receivedDuration = 0;

        engine.setMessageHandler([&] (const char *text, uint32_t duration) {
            called = true;
            receivedText = text;
            receivedDuration = duration;
        });

        engine.showMessage("hello", 1234);

        TEST_ASSERT_TRUE(called);
        TEST_ASSERT_EQUAL_STRING("hello", receivedText);
        TEST_ASSERT_TRUE((receivedDuration) == (uint32_t(1234)));
    
}

void test_16_lock_and_suspend_transitions_update_engine_state_flags() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        TEST_ASSERT_FALSE(engine.isLocked());
        engine.lock();
        TEST_ASSERT_TRUE(engine.isLocked());
        engine.unlock();
        TEST_ASSERT_FALSE(engine.isLocked());

        TEST_ASSERT_FALSE(engine.isSuspended());
        engine.suspend();
        TEST_ASSERT_TRUE(engine.isSuspended());
        engine.resume();
        TEST_ASSERT_FALSE(engine.isSuspended());
    
}

void test_17_recording_state_can_be_toggled_and_set_explicitly() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        engine.setRecording(false);
        TEST_ASSERT_FALSE(engine.recording());

        engine.toggleRecording();
        TEST_ASSERT_TRUE(engine.recording());

        engine.setRecording(false);
        TEST_ASSERT_FALSE(engine.recording());
    
}

void test_18_toggleplay_covers_restart_and_non_shift_start_reset_behavior() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.setMode(ClockSetup::Mode::Master);
        clockSetup.setShiftMode(ClockSetup::ShiftMode::Restart);
        engine.update();

        TEST_ASSERT_FALSE(engine.clockRunning());

        engine.togglePlay(true);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        // Non-shift while running -> reset/stop.
        engine.togglePlay(false);
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        // Non-shift while stopped -> start.
        engine.togglePlay(false);
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());
    
}

void test_19_tap_tempo_and_nudge_helpers_are_callable_and_keep_tempo_positive() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        engine.tapTempoReset();
        harness.waitMs(120);
        engine.tapTempoTap();
        harness.waitMs(120);
        engine.tapTempoTap();

        engine.nudgeTempoSetDirection(1);
        harness.waitMs(20);
        engine.update();

        TEST_ASSERT_TRUE(project.tempo() > 0.f);
        TEST_ASSERT_TRUE(engine.nudgeTempoStrength() >= 0.f);

        engine.nudgeTempoSetDirection(0);
    
}

void test_20_usb_midi_connect_and_disconnect_handlers_are_forwarded() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        int connectCalls = 0;
        int disconnectCalls = 0;
        uint16_t vendorId = 0;
        uint16_t productId = 0;

        engine.setUsbMidiConnectHandler([&] (uint16_t v, uint16_t p) {
            ++connectCalls;
            vendorId = v;
            productId = p;
        });
        engine.setUsbMidiDisconnectHandler([&] () {
            ++disconnectCalls;
        });

        harness.connectUsbMidi(0x1234, 0x5678);
        TEST_ASSERT_TRUE((connectCalls) == (1));
        TEST_ASSERT_TRUE((vendorId) == (uint16_t(0x1234)));
        TEST_ASSERT_TRUE((productId) == (uint16_t(0x5678)));

        harness.disconnectUsbMidi();
        TEST_ASSERT_TRUE((disconnectCalls) == (1));
    
}

void test_21_cv_and_gate_output_overrides_are_applied_during_update() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        engine.setGateOutputOverride(true);
        engine.setGateOutput(0x0f);
        engine.setCvOutputOverride(true);
        engine.setCvOutput(0, 0.25f);
        engine.setCvOutput(1, -0.5f);
        engine.update();

        TEST_ASSERT_TRUE((engine.gateOutput()) == (uint8_t(0x0f)));
        TEST_ASSERT_TRUE(engine.cvOutput().channel(0) == 0.25f);
        TEST_ASSERT_TRUE(engine.cvOutput().channel(1) == -0.5f);

        engine.setGateOutputOverride(false);
        engine.setCvOutputOverride(false);
    
}

void test_22_cv_gate_input_mode_switching_covers_all_converter_branches() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        project.setCvGateInput(Types::CvGateInput::Cv1Cv2);
        engine.update();

        project.setCvGateInput(Types::CvGateInput::Cv3Cv4);
        engine.update();

        project.setCvGateInput(Types::CvGateInput::Off);
        engine.update();

        TEST_ASSERT_TRUE(engine.tick() >= 0);
    
}

void test_23_clock_output_run_mode_and_usb_midi_tx_path_can_run_in_master_mode() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.setMode(ClockSetup::Mode::Master);
        clockSetup.setClockOutputMode(ClockSetup::ClockOutputMode::Run);
        clockSetup.setUsbTx(true);
        clockSetup.setMidiTx(true);
        engine.update();

        engine.clockStart();
        engine.update();
        TEST_ASSERT_TRUE(engine.clockRunning());

        engine.clockStop();
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_24_midi_receive_handler_can_consume_messages_before_monitor_processing() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        harness.app().model.project().track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);
        int handlerCalls = 0;

        engine.setMidiReceiveHandler([&] (MidiPort, uint8_t, const MidiMessage &) {
            ++handlerCalls;
            return true;
        });

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 60, 100));
        engine.update();

        TEST_ASSERT_TRUE((handlerCalls) == (1));
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) == baselineCv);
    
}

void test_25_midi_receive_handler_passthrough_keeps_normal_monitor_processing() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);
        int handlerCalls = 0;

        engine.setMidiReceiveHandler([&] (MidiPort, uint8_t, const MidiMessage &) {
            ++handlerCalls;
            return false;
        });

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 63, 100));
        engine.update();

        TEST_ASSERT_TRUE((handlerCalls) == (1));
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) != baselineCv);
    
}

void test_26_midicv_track_consumption_short_circuits_monitor_path() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = app.engine;
        auto &project = app.model.project();

        // Track 0 consumes as MidiCv; selected track 1 should not be monitored.
        project.setTrackMode(0, Track::TrackMode::MidiCv);
        project.setTrackMode(1, Track::TrackMode::Note);
        project.setSelectedTrackIndex(1);
        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(1).noteTrack().setSlideTime(0);
        engine.update();

        auto &selectedNoteEngine = engine.trackEngine(1).as<NoteTrackEngine>();
        float baselineCv = selectedNoteEngine.cvOutput(0);

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 64, 100));
        engine.update();

        TEST_ASSERT_TRUE(selectedNoteEngine.cvOutput(0) == baselineCv);
    
}

void test_27_midi_input_mode_off_ignores_incoming_monitor_midi() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::Off);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);
        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 60, 100));
        engine.update();

        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) == baselineCv);
    
}

void test_28_midi_source_mode_requires_matching_port_and_channel() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::Source);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        auto &source = project.midiInputSource();
        source.setPort(Types::MidiPort::UsbMidi);
        source.setChannel(2);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);

        harness.sendDinMidi(MidiMessage::makeNoteOn(2, 61, 100));
        engine.update();
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) == baselineCv);

        harness.sendUsbMidi(MidiMessage::makeNoteOn(1, 61, 100));
        engine.update();
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) == baselineCv);

        harness.sendUsbMidi(MidiMessage::makeNoteOn(2, 61, 100));
        engine.update();
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) != baselineCv);
    
}

void test_29_midi_note_on_updates_live_monitor_cv_when_input_mode_allows_it() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);
        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 64, 100));
        engine.update();

        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) != baselineCv);
    
}

void test_30_real_time_and_system_messages_are_ignored_by_midi_monitor_path() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);

        harness.sendDinMidi(MidiMessage(uint8_t(0xF8)));      // Tick (real-time)
        harness.sendDinMidi(MidiMessage(uint8_t(0xF1), 0x01)); // TimeCode (system)
        engine.update();

        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) == baselineCv);
    
}

void test_31_single_byte_din_and_usb_filters_handle_clock_and_non_clock_statuses() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &clockSetup = harness.app().model.project().clockSetup();

        clockSetup.setMode(ClockSetup::Mode::Slave);
        clockSetup.setUsbRx(true);
        engine.update();

        // Clock statuses should be consumed by clock receive filters.
        harness.sendDinMidi(MidiMessage(uint8_t(MidiMessage::Start)));
        harness.sendUsbMidi(MidiMessage(uint8_t(MidiMessage::Tick)));
        engine.update();

        // Non-clock one-byte statuses should not be consumed by clock filters.
        harness.sendDinMidi(MidiMessage(uint8_t(MidiMessage::TuneRequest)));
        harness.sendUsbMidi(MidiMessage(uint8_t(MidiMessage::TuneRequest)));
        engine.update();

        TEST_ASSERT_TRUE(engine.tick() >= 0);
    
}

void test_32_midi_source_mode_with_omni_channel_accepts_matching_port_on_any_channel() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::Source);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        auto &source = project.midiInputSource();
        source.setPort(Types::MidiPort::UsbMidi);
        source.setChannel(-1); // omni
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);
        harness.sendUsbMidi(MidiMessage::makeNoteOn(0, 67, 100));
        engine.update();
        float cvAfterChannel0 = noteEngine.cvOutput(0);
        TEST_ASSERT_TRUE(cvAfterChannel0 != baselineCv);

        harness.sendUsbMidi(MidiMessage::makeNoteOn(9, 60, 100));
        engine.update();
        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) != cvAfterChannel0);
    
}

void test_33_new_note_on_on_same_track_replaces_previous_monitored_note() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();
        auto &noteEngine = engine.trackEngine(0).as<NoteTrackEngine>();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        float baselineCv = noteEngine.cvOutput(0);

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 67, 100));
        engine.update();
        float cvAfterFirstNote = noteEngine.cvOutput(0);
        TEST_ASSERT_TRUE(cvAfterFirstNote != baselineCv);

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 69, 100));
        engine.update();

        TEST_ASSERT_TRUE(noteEngine.cvOutput(0) != cvAfterFirstNote);
    
}

void test_34_note_off_and_control_change_messages_pass_through_monitor_branches() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        engine.update();

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 60, 100));
        engine.update();

        harness.sendDinMidi(MidiMessage::makeControlChange(0, 1, 64));
        engine.update();

        harness.sendDinMidi(MidiMessage::makeNoteOff(0, 60));
        engine.update();

        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_35_changing_selected_track_while_note_is_active_triggers_monitor_handoff() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        project.setMidiInputMode(Types::MidiInputMode::All);
        project.setMonitorMode(Types::MonitorMode::Always);
        project.track(0).noteTrack().setSlideTime(0);
        project.track(1).noteTrack().setSlideTime(0);
        engine.update();

        project.setSelectedTrackIndex(0);
        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 62, 100));
        engine.update();

        project.setSelectedTrackIndex(1);
        harness.sendDinMidi(MidiMessage::makeControlChange(0, 7, 80));
        engine.update();

        TEST_ASSERT_TRUE((project.selectedTrackIndex()) == (1));
    
}

void test_36_fake_note_off_note_on_with_velocity_0_is_normalized_before_receive_handler() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;

        bool handlerCalled = false;
        bool sawNormalizedNoteOff = false;

        engine.setMidiReceiveHandler([&] (MidiPort, uint8_t, const MidiMessage &message) {
            handlerCalled = true;
            sawNormalizedNoteOff = message.isNoteOff();
            return true;
        });

        harness.sendDinMidi(MidiMessage::makeNoteOn(0, 69, 0));
        engine.update();

        TEST_ASSERT_TRUE(handlerCalled);
        TEST_ASSERT_TRUE(sawNormalizedNoteOff);
    
}

void test_37_time_base_and_stats_helpers_expose_consistent_values() {
        SequencerHarness harness;
        auto &engine = harness.app().engine;
        auto &project = harness.app().model.project();

        project.setSyncMeasure(4);
        engine.update();

        TEST_ASSERT_TRUE((engine.noteDivisor()) == (project.timeSignature().noteDivisor()));
        TEST_ASSERT_TRUE((engine.measureDivisor()) == (project.timeSignature().measureDivisor()));
        TEST_ASSERT_TRUE((engine.syncDivisor()) == (uint32_t(project.syncMeasure()) * project.timeSignature().measureDivisor()));

        // At startup no ticks were processed yet, so normalized fractions start at zero.
        TEST_ASSERT_TRUE(engine.measureFraction() == 0.f);
        TEST_ASSERT_TRUE(engine.syncFraction() == 0.f);

        auto stats = engine.stats();
        TEST_ASSERT_TRUE((stats.midiRxOverflow) == (uint32_t(0)));
        TEST_ASSERT_TRUE((stats.usbMidiRxOverflow) == (uint32_t(0)));
    
}

void test_38_gate_offset_zero_triggers_note_on_at_step_start() {
        SequencerHarness harness;
        auto &engine = prepareSingleStepEngine(harness.app(), 0);

        auto result = engine.tick(0);
        TEST_ASSERT_TRUE((result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_39_positive_gate_offset_delays_note_on_by_expected_ticks() {
        SequencerHarness harness;
        constexpr int offset = 3;
        auto &engine = prepareSingleStepEngine(harness.app(), offset);
        uint32_t expectedOnTick = gateOnTick(engine.sequence(), offset);

        for (uint32_t tick = 0; tick < expectedOnTick; ++tick) {
            auto result = engine.tick(tick);
            TEST_ASSERT_FALSE((result & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));
        }

        auto onResult = engine.tick(expectedOnTick);
        TEST_ASSERT_TRUE((onResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_40_gate_off_timing_stays_consistent_when_gate_offset_is_applied() {
        SequencerHarness harness;
        constexpr int offset = NoteSequence::GateOffset::Max;
        constexpr int length = 3;
        auto &engine = prepareSingleStepEngine(harness.app(), offset, length);
        uint32_t onTick = gateOnTick(engine.sequence(), offset);
        uint32_t offTick = onTick + gateLengthTicks(engine.sequence(), length);

        for (uint32_t tick = 0; tick <= onTick; ++tick) {
            engine.tick(tick);
        }
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        for (uint32_t tick = onTick + 1; tick < offTick; ++tick) {
            engine.tick(tick);
        }
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        auto offResult = engine.tick(offTick);
        TEST_ASSERT_TRUE((offResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(engine.gateOutput(0));
    
}

void test_41_consecutive_steps_keep_independent_gate_offset_timing() {
        SequencerHarness harness;
        auto &engine = prepareTwoStepEngine(harness.app(), 0, NoteSequence::GateOffset::Max);
        uint32_t secondStepOnTick = sequenceDivisorTicks(engine.sequence()) +
            gateOnTick(engine.sequence(), NoteSequence::GateOffset::Max);

        auto firstStepResult = engine.tick(0);
        TEST_ASSERT_TRUE((firstStepResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        for (uint32_t tick = 1; tick < secondStepOnTick; ++tick) {
            engine.tick(tick);
        }
        TEST_ASSERT_FALSE(engine.gateOutput(0));

        auto secondStepResult = engine.tick(secondStepOnTick);
        TEST_ASSERT_TRUE((secondStepResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_42_negative_gate_offset_input_is_clamped_to_zero_timing() {
        SequencerHarness harness;
        auto &engine = prepareSingleStepEngine(harness.app(), -5);

        auto result = engine.tick(0);
        TEST_ASSERT_TRUE((result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_track_engines_stay_consistent_after_track_mode_changes);
    RUN_TEST(test_02_track_mode_midicv_is_instantiated_and_can_switch_back_to_note);
    RUN_TEST(test_03_trackenginesconsistent_reports_mismatch_before_setup_update);
    RUN_TEST(test_04_immediate_pattern_request_updates_active_note_sequence);
    RUN_TEST(test_05_latched_mute_request_applies_only_after_commit);
    RUN_TEST(test_06_immediate_playsong_with_invalid_slot_is_ignored_and_request_gets_cleared);
    RUN_TEST(test_07_latched_playsong_applies_only_after_latched_requests_are_committed);
    RUN_TEST(test_08_pattern_change_or_stopsong_request_terminates_song_playback);
    RUN_TEST(test_09_playsong_applies_slot_mutes_when_track_has_mutes_in_song);
    RUN_TEST(test_10_clock_setup_dirty_flag_is_cleared_by_engine_update);
    RUN_TEST(test_11_clock_input_divisor_accepts_common_external_ppqn_values);
    RUN_TEST(test_12_clock_input_divisor_clamping_and_dirty_flag_consumption_are_stable);
    RUN_TEST(test_13_sendmidi_rejects_cvgate_port);
    RUN_TEST(test_14_sendmidi_accepts_midi_and_usb_midi_output_ports);
    RUN_TEST(test_15_message_handler_receives_text_and_duration);
    RUN_TEST(test_16_lock_and_suspend_transitions_update_engine_state_flags);
    RUN_TEST(test_17_recording_state_can_be_toggled_and_set_explicitly);
    RUN_TEST(test_18_toggleplay_covers_restart_and_non_shift_start_reset_behavior);
    RUN_TEST(test_19_tap_tempo_and_nudge_helpers_are_callable_and_keep_tempo_positive);
    RUN_TEST(test_20_usb_midi_connect_and_disconnect_handlers_are_forwarded);
    RUN_TEST(test_21_cv_and_gate_output_overrides_are_applied_during_update);
    RUN_TEST(test_22_cv_gate_input_mode_switching_covers_all_converter_branches);
    RUN_TEST(test_23_clock_output_run_mode_and_usb_midi_tx_path_can_run_in_master_mode);
    RUN_TEST(test_24_midi_receive_handler_can_consume_messages_before_monitor_processing);
    RUN_TEST(test_25_midi_receive_handler_passthrough_keeps_normal_monitor_processing);
    RUN_TEST(test_26_midicv_track_consumption_short_circuits_monitor_path);
    RUN_TEST(test_27_midi_input_mode_off_ignores_incoming_monitor_midi);
    RUN_TEST(test_28_midi_source_mode_requires_matching_port_and_channel);
    RUN_TEST(test_29_midi_note_on_updates_live_monitor_cv_when_input_mode_allows_it);
    RUN_TEST(test_30_real_time_and_system_messages_are_ignored_by_midi_monitor_path);
    RUN_TEST(test_31_single_byte_din_and_usb_filters_handle_clock_and_non_clock_statuses);
    RUN_TEST(test_32_midi_source_mode_with_omni_channel_accepts_matching_port_on_any_channel);
    RUN_TEST(test_33_new_note_on_on_same_track_replaces_previous_monitored_note);
    RUN_TEST(test_34_note_off_and_control_change_messages_pass_through_monitor_branches);
    RUN_TEST(test_35_changing_selected_track_while_note_is_active_triggers_monitor_handoff);
    RUN_TEST(test_36_fake_note_off_note_on_with_velocity_0_is_normalized_before_receive_handler);
    RUN_TEST(test_37_time_base_and_stats_helpers_expose_consistent_values);
    RUN_TEST(test_38_gate_offset_zero_triggers_note_on_at_step_start);
    RUN_TEST(test_39_positive_gate_offset_delays_note_on_by_expected_ticks);
    RUN_TEST(test_40_gate_off_timing_stays_consistent_when_gate_offset_is_applied);
    RUN_TEST(test_41_consecutive_steps_keep_independent_gate_offset_timing);
    RUN_TEST(test_42_negative_gate_offset_input_is_clamped_to_zero_timing);
    return UNITY_END();
}


