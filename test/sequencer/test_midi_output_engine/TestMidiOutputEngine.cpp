/**
 * @file TestMidiOutputEngine.cpp
 * @brief Implements the TestMidiOutputEngine component used by the unit test.
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
#include <vector>

namespace {

class MidiRecorder : public sim::TargetOutputHandler {
public:
    void writeMidiOutput(sim::MidiEvent event) override {
        _events.push_back(event);
    }

    void clear() {
        _events.clear();
    }

    int size() const {
        return int(_events.size());
    }

    const sim::MidiEvent &event(int index) const {
        return _events[index];
    }

private:
    std::vector<sim::MidiEvent> _events;
};

class SequencerHarness {
public:
    SequencerHarness() :
        _simulator(makeTarget())
    {
        _simulator.registerTargetOutputObserver(&_midiRecorder);
        _simulator.reboot();
        _midiRecorder.clear();
    }

    SequencerApp &app() { return *_app; }
    MidiRecorder &midiRecorder() { return _midiRecorder; }

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
    MidiRecorder _midiRecorder;
    sim::Simulator _simulator;
};

static MidiOutput::Output &configureNoteOutput(SequencerApp &app, Types::MidiPort port = Types::MidiPort::UsbMidi, int channel = 2) {
    auto &output = app.model.project().midiOutput().output(0);
    output.clear();
    output.target().setPort(port);
    output.target().setChannel(channel);
    output.setEvent(MidiOutput::Output::Event::Note);
    output.setGateSource(MidiOutput::Output::GateSource::FirstTrack);
    output.setNoteSource(MidiOutput::Output::NoteSource::FirstTrack);
    output.setVelocitySource(MidiOutput::Output::VelocitySource(int(MidiOutput::Output::VelocitySource::FirstVelocity) + 96));
    return output;
}

static MidiOutput::Output &configureFixedNoteOutput(SequencerApp &app, Types::MidiPort port = Types::MidiPort::Midi, int channel = 1) {
    auto &output = app.model.project().midiOutput().output(0);
    output.clear();
    output.target().setPort(port);
    output.target().setChannel(channel);
    output.setEvent(MidiOutput::Output::Event::Note);
    output.setGateSource(MidiOutput::Output::GateSource::FirstTrack);
    output.setNoteSource(MidiOutput::Output::NoteSource(int(MidiOutput::Output::NoteSource::FirstNote) + 64));
    output.setVelocitySource(MidiOutput::Output::VelocitySource(int(MidiOutput::Output::VelocitySource::FirstVelocity) + 127));
    return output;
}

static MidiOutput::Output &configureControlOutput(SequencerApp &app, Types::MidiPort port = Types::MidiPort::UsbMidi, int channel = 3) {
    auto &output = app.model.project().midiOutput().output(0);
    output.clear();
    output.target().setPort(port);
    output.target().setChannel(channel);
    output.setEvent(MidiOutput::Output::Event::ControlChange);
    output.setControlNumber(74);
    output.setControlSource(MidiOutput::Output::ControlSource::FirstTrack);
    return output;
}

static void expectMessagePort(const sim::MidiEvent &event, int port) {
    TEST_ASSERT_TRUE((event.kind) == (int(sim::MidiEvent::Message)));
    TEST_ASSERT_TRUE((event.port) == (port));
}

static void expectNoteOn(const sim::MidiEvent &event, int port, int channel, int note, int velocity) {
    expectMessagePort(event, port);
    TEST_ASSERT_TRUE(event.message.isNoteOn());
    TEST_ASSERT_TRUE((int(event.message.channel())) == (channel));
    TEST_ASSERT_TRUE((int(event.message.note())) == (note));
    TEST_ASSERT_TRUE((int(event.message.velocity())) == (velocity));
}

static void expectNoteOff(const sim::MidiEvent &event, int port, int channel, int note) {
    expectMessagePort(event, port);
    TEST_ASSERT_TRUE(event.message.isNoteOff());
    TEST_ASSERT_TRUE((int(event.message.channel())) == (channel));
    TEST_ASSERT_TRUE((int(event.message.note())) == (note));
}

static void expectControlChange(const sim::MidiEvent &event, int port, int channel, int controlNumber, int controlValue) {
    expectMessagePort(event, port);
    TEST_ASSERT_TRUE(event.message.isControlChange());
    TEST_ASSERT_TRUE((int(event.message.channel())) == (channel));
    TEST_ASSERT_TRUE((int(event.message.controlNumber())) == (controlNumber));
    TEST_ASSERT_TRUE((int(event.message.controlValue())) == (controlValue));
}

} // namespace

void test_01_fixed_note_and_velocity_sources_produce_note_on_and_note_off_messages() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureFixedNoteOutput(harness.app());
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendGate(0, true);
        midiOutputEngine.update();

        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectNoteOn(harness.midiRecorder().event(0), 0, 1, 64, 127);

        harness.midiRecorder().clear();
        midiOutputEngine.sendGate(0, false);
        midiOutputEngine.update();

        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectNoteOff(harness.midiRecorder().event(0), 0, 1, 64);
    
}

void test_02_track_driven_note_output_suppresses_tied_retriggers_and_replaces_active_notes_when_pitch_changes() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureNoteOutput(harness.app());
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendCv(0, 1.f);
        midiOutputEngine.sendGate(0, true);
        midiOutputEngine.update();

        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectNoteOn(harness.midiRecorder().event(0), 1, 2, 72, 96);

        harness.midiRecorder().clear();
        midiOutputEngine.sendGate(0, false);
        midiOutputEngine.sendGate(0, true);
        midiOutputEngine.update();
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (0));

        midiOutputEngine.sendCv(0, 2.f);
        midiOutputEngine.sendGate(0, true);
        midiOutputEngine.update();

        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (2));
        expectNoteOn(harness.midiRecorder().event(0), 1, 2, 84, 96);
        expectNoteOff(harness.midiRecorder().event(1), 1, 2, 72);
    
}

void test_03_slide_requests_emit_portamento_control_changes_only_when_the_state_changes() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureNoteOutput(harness.app());
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendSlide(0, true);
        midiOutputEngine.update();
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectControlChange(harness.midiRecorder().event(0), 1, 2, 65, 127);

        harness.midiRecorder().clear();
        midiOutputEngine.sendSlide(0, true);
        midiOutputEngine.update();
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (0));

        midiOutputEngine.sendSlide(0, false);
        midiOutputEngine.update();
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectControlChange(harness.midiRecorder().event(0), 1, 2, 65, 0);
    
}

void test_04_control_change_output_does_not_send_immediately_without_force() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureControlOutput(harness.app());
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendCv(0, 2.5f);
        midiOutputEngine.update();
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (0));
    
}

void test_05_forced_control_change_flush_sends_changed_values_and_ignores_unchanged_values() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureControlOutput(harness.app());
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendCv(0, 2.5f);
        midiOutputEngine.update(true);
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectControlChange(harness.midiRecorder().event(0), 1, 3, 74, 63);

        harness.midiRecorder().clear();
        midiOutputEngine.sendCv(0, 2.5f);
        midiOutputEngine.update(true);
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (0));

        midiOutputEngine.sendCv(0, 9.f);
        midiOutputEngine.update(true);
        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (1));
        expectControlChange(harness.midiRecorder().event(0), 1, 3, 74, 127);
    
}

void test_06_reset_sends_note_cleanup_for_active_note_outputs() {
        SequencerHarness harness;
        auto &midiOutputEngine = harness.app().engine.midiOutputEngine();
        configureNoteOutput(harness.app(), Types::MidiPort::UsbMidi, 4);
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.sendCv(0, 1.f);
        midiOutputEngine.sendSlide(0, true);
        midiOutputEngine.sendGate(0, true);
        midiOutputEngine.update();
        harness.midiRecorder().clear();

        midiOutputEngine.reset();

        TEST_ASSERT_TRUE((harness.midiRecorder().size()) == (3));
        expectNoteOff(harness.midiRecorder().event(0), 1, 4, 72);
        expectControlChange(harness.midiRecorder().event(1), 1, 4, 65, 0);
        expectControlChange(harness.midiRecorder().event(2), 1, 4, 120, 0);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_fixed_note_and_velocity_sources_produce_note_on_and_note_off_messages);
    RUN_TEST(test_02_track_driven_note_output_suppresses_tied_retriggers_and_replaces_active_notes_when_pitch_changes);
    RUN_TEST(test_03_slide_requests_emit_portamento_control_changes_only_when_the_state_changes);
    RUN_TEST(test_04_control_change_output_does_not_send_immediately_without_force);
    RUN_TEST(test_05_forced_control_change_flush_sends_changed_values_and_ignores_unchanged_values);
    RUN_TEST(test_06_reset_sends_note_cleanup_for_active_note_outputs);
    return UNITY_END();
}






