/**
 * @file TestRoutingEngine.cpp
 * @brief Implements the TestRoutingEngine component used by the unit test.
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
    }

    SequencerApp &app() { return *_app; }

    void setCvInput(int channel, float volts) {
        _simulator.setAdc(channel, volts);
    }

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

static uint8_t allTrackMask() {
    return uint8_t((1u << CONFIG_TRACK_COUNT) - 1u);
}

static void clearRoutedFlags() {
    for (int target = 0; target < int(Routing::Target::Last); ++target) {
        Routing::setRouted(Routing::Target(target), allTrackMask(), false);
    }
}

static Routing::Route &configureMidiRoute(
    SequencerApp &app,
    int routeIndex,
    Routing::Target target,
    uint8_t tracks,
    Routing::MidiSource::Event event)
{
    auto &route = app.model.project().routing().route(routeIndex);
    route.clear();
    route.setTarget(target);
    route.setTracks(tracks);
    route.setSource(Routing::Source::Midi);
    route.midiSource().setEvent(event);
    return route;
}

static Routing::Route &configureCvRoute(
    SequencerApp &app,
    int routeIndex,
    Routing::Target target,
    uint8_t tracks,
    Routing::Source source,
    Types::VoltageRange range = Types::VoltageRange::Unipolar5V)
{
    auto &route = app.model.project().routing().route(routeIndex);
    route.clear();
    route.setTarget(target);
    route.setTracks(tracks);
    route.setSource(source);
    route.cvSource().setRange(range);
    return route;
}

static void updateApp(SequencerApp &app, int count = 1) {
    for (int i = 0; i < count; ++i) {
        app.update();
    }
}

} // namespace

void test_01_control_absolute_and_relative_midi_routes_consume_matching_messages_and_keep_midi_state() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &routingEngine = app.engine.routingEngine();
        auto &playState = app.model.project().playState();

        auto &absoluteRoute = configureMidiRoute(
            app,
            0,
            Routing::Target::FillAmount,
            uint8_t(1 << 0),
            Routing::MidiSource::Event::ControlAbsolute);
        absoluteRoute.midiSource().setControlNumber(10);

        auto &relativeRoute = configureMidiRoute(
            app,
            1,
            Routing::Target::FillAmount,
            uint8_t(1 << 1),
            Routing::MidiSource::Event::ControlRelative);
        relativeRoute.midiSource().setControlNumber(11);

        TEST_ASSERT_FALSE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 12, 100)));
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(0).fillAmount()) == (0));

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 10, 127)));
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(0).fillAmount()) == (100));

        // MIDI sources are held until the next matching event arrives.
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(0).fillAmount()) == (100));

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 11, 1)));
        routingEngine.update();
        TEST_ASSERT_TRUE(playState.trackState(1).fillAmount() > 0);

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 11, 127)));
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(1).fillAmount()) == (0));
    
}

void test_02_pitch_bend_and_note_based_midi_routes_drive_their_configured_targets() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &routingEngine = app.engine.routingEngine();
        auto &project = app.model.project();
        auto &playState = project.playState();
        const MidiMessage maxPitchBend(uint8_t(MidiMessage::PitchBend | 2), 0x7f, 0x7f);

        auto &pitchBendRoute = configureMidiRoute(
            app,
            0,
            Routing::Target::Swing,
            0,
            Routing::MidiSource::Event::PitchBend);
        pitchBendRoute.midiSource().source().setPort(Types::MidiPort::UsbMidi);
        pitchBendRoute.midiSource().source().setChannel(2);

        auto &momentaryRoute = configureMidiRoute(
            app,
            1,
            Routing::Target::Fill,
            uint8_t(1 << 0),
            Routing::MidiSource::Event::NoteMomentary);
        momentaryRoute.midiSource().setNote(60);

        auto &toggleRoute = configureMidiRoute(
            app,
            2,
            Routing::Target::RecordToggle,
            0,
            Routing::MidiSource::Event::NoteToggle);
        toggleRoute.midiSource().setNote(61);

        auto &velocityRoute = configureMidiRoute(
            app,
            3,
            Routing::Target::FillAmount,
            uint8_t(1 << 0),
            Routing::MidiSource::Event::NoteVelocity);
        velocityRoute.midiSource().setNote(62);

        auto &rangeRoute = configureMidiRoute(
            app,
            4,
            Routing::Target::Pattern,
            uint8_t(1 << 0),
            Routing::MidiSource::Event::NoteRange);
        rangeRoute.midiSource().setNote(60);
        rangeRoute.midiSource().setNoteRange(4);

        TEST_ASSERT_FALSE(routingEngine.receiveMidi(MidiPort::Midi, maxPitchBend));
        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::UsbMidi, maxPitchBend));
        routingEngine.update();
        TEST_ASSERT_TRUE((project.swing()) == (75));

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 60, 100)));
        routingEngine.update();
        TEST_ASSERT_TRUE(playState.trackState(0).fill());

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOff(0, 60, 0)));
        routingEngine.update();
        TEST_ASSERT_FALSE(playState.trackState(0).fill());

        TEST_ASSERT_FALSE(app.engine.recording());
        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61, 100)));
        routingEngine.update();
        TEST_ASSERT_TRUE(app.engine.recording());

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61, 100)));
        routingEngine.update();
        TEST_ASSERT_TRUE(app.engine.recording());

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 61, 100)));
        routingEngine.update();
        TEST_ASSERT_FALSE(app.engine.recording());

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 62, 64)));
        routingEngine.update();
        TEST_ASSERT_TRUE(playState.trackState(0).fillAmount() > 0);
        TEST_ASSERT_TRUE(playState.trackState(0).fillAmount() < 100);

        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeNoteOn(0, 63, 100)));
        routingEngine.update();
        TEST_ASSERT_TRUE((project.selectedPatternIndex()) == (15));
    
}

void test_03_cv_input_cv_output_and_none_sources_normalize_into_routed_targets() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &routingEngine = app.engine.routingEngine();
        auto &playState = app.model.project().playState();

        configureCvRoute(app, 0, Routing::Target::FillAmount, uint8_t(1 << 0), Routing::Source::CvIn1);
        harness.setCvInput(0, 5.f);
        updateApp(app);
        TEST_ASSERT_TRUE((playState.trackState(0).fillAmount()) == (100));

        configureCvRoute(app, 1, Routing::Target::FillAmount, uint8_t(1 << 1), Routing::Source::CvOut1);
        app.engine.setCvOutputOverride(true);
        app.engine.setCvOutput(0, 5.f);
        updateApp(app);
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(1).fillAmount()) == (100));

        auto &noneRoute = configureMidiRoute(
            app,
            2,
            Routing::Target::FillAmount,
            uint8_t(1 << 2),
            Routing::MidiSource::Event::ControlAbsolute);
        noneRoute.midiSource().setControlNumber(20);
        TEST_ASSERT_TRUE(routingEngine.receiveMidi(MidiPort::Midi, MidiMessage::makeControlChange(0, 20, 127)));
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(2).fillAmount()) == (100));

        noneRoute.setSource(Routing::Source::None);
        routingEngine.update();
        TEST_ASSERT_TRUE((playState.trackState(2).fillAmount()) == (0));
    
}

void test_04_play_and_record_level_targets_follow_active_state_changes_without_double_toggles() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = app.engine;

        configureCvRoute(app, 0, Routing::Target::Play, 0, Routing::Source::CvIn1);
        configureCvRoute(app, 1, Routing::Target::Record, 0, Routing::Source::CvIn2);

        harness.setCvInput(0, 5.f);
        harness.setCvInput(1, 5.f);
        updateApp(app, 2);
        TEST_ASSERT_TRUE(engine.clockRunning());
        TEST_ASSERT_TRUE(engine.recording());

        updateApp(app, 2);
        TEST_ASSERT_TRUE(engine.clockRunning());
        TEST_ASSERT_TRUE(engine.recording());

        harness.setCvInput(0, 0.f);
        harness.setCvInput(1, 0.f);
        updateApp(app, 2);
        TEST_ASSERT_FALSE(engine.clockRunning());
        TEST_ASSERT_FALSE(engine.recording());
    
}

void test_05_route_changes_clear_previous_routed_flags_and_reset_play_toggle_edge_state() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = app.engine;
        auto &route = configureCvRoute(app, 0, Routing::Target::PlayToggle, 0, Routing::Source::CvIn1);

        harness.setCvInput(0, 5.f);
        updateApp(app, 2);
        TEST_ASSERT_TRUE(engine.clockRunning());
        TEST_ASSERT_TRUE(Routing::isRouted(Routing::Target::PlayToggle));

        route.setTarget(Routing::Target::Swing);
        route.setSource(Routing::Source::None);
        app.engine.routingEngine().update();
        TEST_ASSERT_FALSE(Routing::isRouted(Routing::Target::PlayToggle));
        TEST_ASSERT_TRUE(Routing::isRouted(Routing::Target::Swing));

        route.setTarget(Routing::Target::PlayToggle);
        route.setSource(Routing::Source::CvIn1);
        route.cvSource().setRange(Types::VoltageRange::Unipolar5V);
        app.engine.routingEngine().update();
        updateApp(app);
        TEST_ASSERT_FALSE(engine.clockRunning());
    
}

void test_06_record_toggle_reroute_resets_edge_tracking_so_a_restored_high_input_can_toggle_again() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = app.engine;
        auto &route = configureCvRoute(app, 0, Routing::Target::RecordToggle, 0, Routing::Source::CvIn1);

        harness.setCvInput(0, 5.f);
        updateApp(app, 2);
        TEST_ASSERT_TRUE(engine.recording());

        route.setTarget(Routing::Target::Swing);
        route.setSource(Routing::Source::None);
        app.engine.routingEngine().update();
        TEST_ASSERT_FALSE(Routing::isRouted(Routing::Target::RecordToggle));

        route.setTarget(Routing::Target::RecordToggle);
        route.setSource(Routing::Source::CvIn1);
        app.engine.routingEngine().update();
        updateApp(app);
        TEST_ASSERT_FALSE(engine.recording());
    
}

void test_07_tap_tempo_reacts_only_on_rising_edges_of_an_active_routing_source() {
        clearRoutedFlags();
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();

        configureCvRoute(app, 0, Routing::Target::TapTempo, 0, Routing::Source::CvIn1);

        const float baselineTempo = project.tempo();

        // First rising edge initializes tap tempo state only.
        harness.setCvInput(0, 5.f);
        updateApp(app, 2);
        harness.waitMs(300);
        updateApp(app, 2);
        TEST_ASSERT_TRUE((project.tempo()) == (baselineTempo));

        // Keeping the source high must not retrigger tap tempo.
        harness.waitMs(200);
        updateApp(app, 2);
        TEST_ASSERT_TRUE((project.tempo()) == (baselineTempo));

        // Subsequent low->high transitions must remain edge-triggered and keep the project valid.
        harness.setCvInput(0, 0.f);
        updateApp(app, 2);
        harness.waitMs(180);
        harness.setCvInput(0, 5.f);
        updateApp(app, 2);

        harness.setCvInput(0, 0.f);
        updateApp(app, 2);
        harness.waitMs(180);
        harness.setCvInput(0, 5.f);
        updateApp(app, 2);

        TEST_ASSERT_TRUE(Routing::isRouted(Routing::Target::TapTempo));
        TEST_ASSERT_TRUE(project.tempo() >= 1.f && project.tempo() <= 1000.f);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_control_absolute_and_relative_midi_routes_consume_matching_messages_and_keep_midi_state);
    RUN_TEST(test_02_pitch_bend_and_note_based_midi_routes_drive_their_configured_targets);
    RUN_TEST(test_03_cv_input_cv_output_and_none_sources_normalize_into_routed_targets);
    RUN_TEST(test_04_play_and_record_level_targets_follow_active_state_changes_without_double_toggles);
    RUN_TEST(test_05_route_changes_clear_previous_routed_flags_and_reset_play_toggle_edge_state);
    RUN_TEST(test_06_record_toggle_reroute_resets_edge_tracking_so_a_restored_high_input_can_toggle_again);
    RUN_TEST(test_07_tap_tempo_reacts_only_on_rising_edges_of_an_active_routing_source);
    return UNITY_END();
}

