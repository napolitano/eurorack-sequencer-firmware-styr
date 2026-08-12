/**
 * @file TestMidiCvTrackEngine.cpp
 * @brief Implements the TestMidiCvTrackEngine component used by the unit test.
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

#include "core/Simulator.h"

#include <memory>

namespace {

class SequencerHarness {
public:
    SequencerHarness() :
        _simulator(makeTarget())
    {
        _simulator.reboot();
        _simulator.wait(1);
    }

    SequencerApp &app() { return *_app; }

    void waitMs(int ms) {
        _simulator.wait(ms);
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

static MidiCvTrackEngine &configureMidiCvTrack(SequencerApp &app, int trackIndex) {
    auto &project = app.model.project();
    project.setTrackMode(trackIndex, Track::TrackMode::MidiCv);
    app.engine.update();

    auto &track = project.track(trackIndex);
    auto &midiCvTrack = track.midiCvTrack();
    midiCvTrack.clear();
    midiCvTrack.source().setPort(Types::MidiPort::Midi);
    midiCvTrack.source().setChannel(-1);
    midiCvTrack.setVoices(1);
    midiCvTrack.setVoiceConfig(MidiCvTrack::VoiceConfig::Pitch);
    midiCvTrack.setNotePriority(MidiCvTrack::NotePriority::LastNote);
    midiCvTrack.setLowNote(0);
    midiCvTrack.setHighNote(127);
    midiCvTrack.setPitchBendRange(12);
    midiCvTrack.setModulationRange(Types::VoltageRange::Unipolar5V);
    midiCvTrack.setRetrigger(false);
    midiCvTrack.setSlideTime(0);
    midiCvTrack.setTranspose(0);
    midiCvTrack.arpeggiator().clear();

    auto &engine = app.engine.trackEngine(trackIndex).as<MidiCvTrackEngine>();
    engine.reset();
    return engine;
}

static float noteToCv(int note) {
    return (note - 60) * (1.f / 12.f);
}

static float valueToCv(int value, Types::VoltageRange range = Types::VoltageRange::Unipolar5V) {
    return Types::voltageRangeInfo(range).denormalize(value * (1.f / 127.f));
}

static float pitchBendToCv(int value, int bendRange) {
    return value * bendRange * (1.f / (12 * 8192));
}

} // namespace

void test_01_reset_restart_tick_and_basic_note_handling_keep_engine_state_coherent() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        TEST_ASSERT_TRUE((engine.tick(0)) == (TrackEngine::TickResult::NoUpdate));
        TEST_ASSERT_FALSE(engine.activity());
        TEST_ASSERT_FALSE(engine.gateOutput(0));
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (0.f));

        track.setLowNote(60);
        track.setHighNote(72);

        TEST_ASSERT_FALSE(engine.receiveMidi(MidiPort::UsbMidi, MidiMessage::makeNoteOn(0, 60, 100)));
        TEST_ASSERT_FALSE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 59, 100)));
        TEST_ASSERT_FALSE(engine.activity());

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        TEST_ASSERT_TRUE(engine.activity());
        TEST_ASSERT_TRUE(engine.gateOutput(0));
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (noteToCv(60)));

        // restart() is intentionally a no-op for MIDI/CV tracks.
        engine.restart();
        TEST_ASSERT_TRUE(engine.activity());
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOff(0, 60, 0)));
        TEST_ASSERT_FALSE(engine.activity());
        TEST_ASSERT_FALSE(engine.gateOutput(0));

        engine.reset();
        TEST_ASSERT_FALSE(engine.activity());
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (0.f));
    
}

void test_02_pitch_velocity_pressure_channel_pressure_and_pitch_bend_drive_configured_cv_outputs() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();
        const MidiMessage maxPitchBend(uint8_t(MidiMessage::PitchBend | 0), 0x7f, 0x7f);

        track.setVoiceConfig(MidiCvTrack::VoiceConfig::PitchVelocityPressure);
        track.setPitchBendRange(12);
        track.setModulationRange(Types::VoltageRange::Unipolar5V);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 72, 64)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeKeyPressure(0, 72, 32)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeChannelPressure(0, 16)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, maxPitchBend));
        engine.update(0.f);

        const float expectedPitch = noteToCv(72) + pitchBendToCv(8191, 12);
        const float expectedVelocity = valueToCv(64);
        const float expectedPressure = valueToCv(32) + valueToCv(16);

        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (expectedPitch));
        TEST_ASSERT_TRUE((engine.cvOutput(1)) == (expectedVelocity));
        TEST_ASSERT_TRUE((engine.cvOutput(2)) == (expectedPressure));

        // Key pressure on an inactive note is still consumed but must not change activity.
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeKeyPressure(0, 71, 40)));
        TEST_ASSERT_TRUE(engine.activity());

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOff(0, 72, 0)));
        TEST_ASSERT_FALSE(engine.activity());
    
}

void test_03_note_priority_lastnote_keeps_the_newest_note_on_the_monophonic_output() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        harness.waitMs(1);
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 64, 100)));
        engine.update(0.f);

        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (noteToCv(64)));
    
}

void test_04_note_priority_firstnote_keeps_the_oldest_note_on_the_monophonic_output() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();
        track.setNotePriority(MidiCvTrack::NotePriority::FirstNote);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        harness.waitMs(1);
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 64, 100)));
        engine.update(0.f);

        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (noteToCv(60)));
    
}

void test_05_note_priority_lowestnote_and_highestnote_pick_the_expected_monophonic_note() {
        {
            SequencerHarness lowHarness;
            auto &lowEngine = configureMidiCvTrack(lowHarness.app(), 0);
            auto &lowTrack = lowHarness.app().model.project().track(0).midiCvTrack();
            lowTrack.setNotePriority(MidiCvTrack::NotePriority::LowestNote);
            TEST_ASSERT_TRUE(lowEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 67, 100)));
            TEST_ASSERT_TRUE(lowEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
            lowEngine.update(0.f);
            TEST_ASSERT_TRUE((lowEngine.cvOutput(0)) == (noteToCv(60)));
        }

        {
            SequencerHarness highHarness;
            auto &highEngine = configureMidiCvTrack(highHarness.app(), 0);
            auto &highTrack = highHarness.app().model.project().track(0).midiCvTrack();
            highTrack.setNotePriority(MidiCvTrack::NotePriority::HighestNote);
            TEST_ASSERT_TRUE(highEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
            TEST_ASSERT_TRUE(highEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 67, 100)));
            highEngine.update(0.f);
            TEST_ASSERT_TRUE((highEngine.cvOutput(0)) == (noteToCv(67)));
        }
    
}

void test_06_retrigger_delays_gate_reactivation_briefly_after_note_on() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();
        track.setRetrigger(true);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        TEST_ASSERT_FALSE(engine.gateOutput(0));

        harness.waitMs(5);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_07_monophonic_slide_applies_smoothing_when_a_new_note_arrives_while_another_voice_is_active() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        track.setSlideTime(100);
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        engine.update(0.f);
        float startCv = engine.cvOutput(0);

        harness.waitMs(1);
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 72, 100)));
        engine.update(0.001f);
        float smoothedCv = engine.cvOutput(0);

        TEST_ASSERT_TRUE(smoothedCv > startCv);
        TEST_ASSERT_TRUE(smoothedCv < noteToCv(72));

        engine.update(1.f);
        TEST_ASSERT_TRUE(engine.cvOutput(0) > smoothedCv);
    
}

void test_08_polyphonic_outputs_expose_pitch_velocity_and_pressure_signals_per_allocated_voice() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        track.setVoices(2);
        track.setVoiceConfig(MidiCvTrack::VoiceConfig::PitchVelocityPressure);
        track.setModulationRange(Types::VoltageRange::Unipolar5V);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 32)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeKeyPressure(0, 60, 10)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 67, 96)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeKeyPressure(0, 67, 20)));

        bool foundLowPitch = false;
        bool foundHighPitch = false;
        bool foundLowVelocity = false;
        bool foundHighVelocity = false;
        bool foundLowPressure = false;
        bool foundHighPressure = false;

        for (int index = 0; index < 6; ++index) {
            float value = engine.cvOutput(index);
            if (value == noteToCv(60)) foundLowPitch = true;
            if (value == noteToCv(67)) foundHighPitch = true;
            if (value == valueToCv(32)) foundLowVelocity = true;
            if (value == valueToCv(96)) foundHighVelocity = true;
            if (value == valueToCv(10)) foundLowPressure = true;
            if (value == valueToCv(20)) foundHighPressure = true;
        }

        TEST_ASSERT_TRUE(foundLowPitch);
        TEST_ASSERT_TRUE(foundHighPitch);
        TEST_ASSERT_TRUE(foundLowVelocity);
        TEST_ASSERT_TRUE(foundHighVelocity);
        TEST_ASSERT_TRUE(foundLowPressure);
        TEST_ASSERT_TRUE(foundHighPressure);
    
}

void test_09_adding_more_than_eight_active_notes_replaces_the_lowest_priority_internal_voice() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        track.setVoices(8);
        track.setVoiceConfig(MidiCvTrack::VoiceConfig::Pitch);
        track.setNotePriority(MidiCvTrack::NotePriority::LastNote);

        for (int note = 60; note < 68; ++note) {
            TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, note, 100)));
            harness.waitMs(1);
        }

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 72, 100)));

        bool foundOldestNote = false;
        bool foundNewestNote = false;
        for (int index = 0; index < 8; ++index) {
            float value = engine.cvOutput(index);
            if (value == noteToCv(60)) foundOldestNote = true;
            if (value == noteToCv(72)) foundNewestNote = true;
        }

        TEST_ASSERT_FALSE(foundOldestNote);
        TEST_ASSERT_TRUE(foundNewestNote);
    
}

void test_10_arpeggiator_tick_path_turns_held_notes_into_active_output_voices_and_disable_resets_them() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        track.arpeggiator().setEnabled(true);
        track.arpeggiator().setMode(Arpeggiator::Mode::PlayOrder);
        track.arpeggiator().setDivisor(12);
        track.arpeggiator().setGateLength(50);
        engine.update(0.f);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 64, 100)));
        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOff(0, 64, 0)));
        TEST_ASSERT_FALSE(engine.activity());

        TEST_ASSERT_TRUE((engine.tick(0)) == (TrackEngine::TickResult::NoUpdate));
        TEST_ASSERT_TRUE(engine.activity());
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        track.arpeggiator().setEnabled(false);
        engine.reset();
        engine.update(0.f);
        TEST_ASSERT_FALSE(engine.activity());
        TEST_ASSERT_FALSE(engine.gateOutput(0));
    
}

void test_11_arpeggiator_queued_note_off_event_clears_activity_after_the_configured_gate_length() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();

        track.arpeggiator().setEnabled(true);
        track.arpeggiator().setMode(Arpeggiator::Mode::PlayOrder);
        track.arpeggiator().setDivisor(12);
        track.arpeggiator().setGateLength(50);
        engine.update(0.f);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        TEST_ASSERT_TRUE((engine.tick(0)) == (TrackEngine::TickResult::NoUpdate));
        TEST_ASSERT_TRUE(engine.activity());
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        TEST_ASSERT_TRUE((engine.tick(24)) == (TrackEngine::TickResult::NoUpdate));
        TEST_ASSERT_FALSE(engine.activity());
        TEST_ASSERT_FALSE(engine.gateOutput(0));
    
}

void test_12_arpeggiator_update_advances_while_clock_is_stopped() {
        SequencerHarness harness;
        auto &engine = configureMidiCvTrack(harness.app(), 0);
        auto &track = harness.app().model.project().track(0).midiCvTrack();
        const float tickDuration = harness.app().engine.clock().tickDuration();

        track.arpeggiator().setEnabled(true);
        track.arpeggiator().setMode(Arpeggiator::Mode::PlayOrder);
        track.arpeggiator().setDivisor(12);
        engine.update(0.f);

        TEST_ASSERT_TRUE(engine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 72, 100)));
        TEST_ASSERT_FALSE(engine.activity());

        engine.update(tickDuration * 2.f);
        TEST_ASSERT_TRUE(engine.activity());
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (noteToCv(72)));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_reset_restart_tick_and_basic_note_handling_keep_engine_state_coherent);
    RUN_TEST(test_02_pitch_velocity_pressure_channel_pressure_and_pitch_bend_drive_configured_cv_outputs);
    RUN_TEST(test_03_note_priority_lastnote_keeps_the_newest_note_on_the_monophonic_output);
    RUN_TEST(test_04_note_priority_firstnote_keeps_the_oldest_note_on_the_monophonic_output);
    RUN_TEST(test_05_note_priority_lowestnote_and_highestnote_pick_the_expected_monophonic_note);
    RUN_TEST(test_06_retrigger_delays_gate_reactivation_briefly_after_note_on);
    RUN_TEST(test_07_monophonic_slide_applies_smoothing_when_a_new_note_arrives_while_another_voice_is_active);
    RUN_TEST(test_08_polyphonic_outputs_expose_pitch_velocity_and_pressure_signals_per_allocated_voice);
    RUN_TEST(test_09_adding_more_than_eight_active_notes_replaces_the_lowest_priority_internal_voice);
    RUN_TEST(test_10_arpeggiator_tick_path_turns_held_notes_into_active_output_voices_and_disable_resets_them);
    RUN_TEST(test_11_arpeggiator_queued_note_off_event_clears_activity_after_the_configured_gate_length);
    RUN_TEST(test_12_arpeggiator_update_advances_while_clock_is_stopped);
    return UNITY_END();
}


