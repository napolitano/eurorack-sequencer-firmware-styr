/**
 * @file TestNoteTrackEngineAdvanced.cpp
 * @brief Implements the TestNoteTrackEngineAdvanced component used by the unit test.
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

static NoteTrackEngine &prepareTwoStepSequence(SequencerApp &app) {
    auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
    sequence.clearSteps();
    sequence.setFirstStep(0);
    sequence.setLastStep(1);

    for (int stepIndex = 0; stepIndex <= 1; ++stepIndex) {
        auto &step = sequence.step(stepIndex);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(NoteSequence::Length::Max / 2);
        step.setLengthVariationRange(0);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setGateOffset(0);
    }

    auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();
    engine.reset();
    return engine;
}

static NoteTrackEngine &prepareSingleStepSequence(SequencerApp &app, int trackIndex) {
    auto &sequence = app.model.project().track(trackIndex).noteTrack().sequence(0);
    sequence.clearSteps();
    sequence.setFirstStep(0);
    sequence.setLastStep(0);

    auto &step = sequence.step(0);
    step.setGate(true);
    step.setGateProbability(NoteSequence::GateProbability::Max);
    step.setLength(0);
    step.setLengthVariationRange(0);
    step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
    step.setRetrigger(0);
    step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
    step.setGateOffset(0);

    auto &engine = app.engine.trackEngine(trackIndex).as<NoteTrackEngine>();
    engine.reset();
    return engine;
}

static void tickRange(NoteTrackEngine &engine, uint32_t firstTickInclusive, uint32_t lastTickInclusive) {
    for (uint32_t tick = firstTickInclusive; tick <= lastTickInclusive; ++tick) {
        engine.tick(tick);
    }
}

} // namespace

void test_01_playmode_free_advances_on_internal_free_tick_cycle() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = prepareSingleStepSequence(app, 0);
        auto &sequence = project.track(0).noteTrack().sequence(0);

        project.track(0).noteTrack().setPlayMode(Types::PlayMode::Free);
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto first = engine.tick(0);
        TEST_ASSERT_TRUE((first & TrackEngine::TickResult::GateUpdate) != 0);

        for (uint32_t tick = 1; tick < divisor; ++tick) {
            auto r = engine.tick(tick);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));
        }

        auto nextCycle = engine.tick(divisor);
        TEST_ASSERT_TRUE((nextCycle & TrackEngine::TickResult::GateUpdate) != 0);
    
}

void test_02_single_step_sequence_reports_finite_zero_progress() {
        SequencerHarness harness;
        auto &engine = prepareSingleStepSequence(harness.app(), 0);

        engine.tick(0);
        TEST_ASSERT_TRUE((engine.currentStep()) == (0));
        TEST_ASSERT_TRUE((engine.sequenceProgress()) == (0.f));
    
}

void test_03_restart_clears_current_step_after_sequence_was_advanced() {
        SequencerHarness harness;
        auto &engine = prepareTwoStepSequence(harness.app());

        auto result = engine.tick(0);
        TEST_ASSERT_TRUE((result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE((engine.currentStep()) == (0));

        engine.restart();
        TEST_ASSERT_TRUE((engine.currentStep()) == (-1));
    
}

void test_04_setmonitorstep_drives_step_recorder_index_only_in_step_record_mode() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = app.engine;
        auto &noteEngine = prepareTwoStepSequence(app);

        project.setSelectedTrackIndex(0);
        project.setRecordMode(Types::RecordMode::StepRecord);
        engine.setRecording(true);

        // update() enables step recorder and initializes to first step.
        noteEngine.update(0.f);
        TEST_ASSERT_TRUE((noteEngine.currentRecordStep()) == (0));

        noteEngine.setMonitorStep(1);
        TEST_ASSERT_TRUE((noteEngine.currentRecordStep()) == (1));

        // Invalid monitor index must not alter recorder step.
        noteEngine.setMonitorStep(-1);
        TEST_ASSERT_TRUE((noteEngine.currentRecordStep()) == (1));

        engine.setRecording(false);
        noteEngine.update(0.f);
        TEST_ASSERT_TRUE((noteEngine.currentRecordStep()) == (-1));
    
}

void test_05_monitor_step_override_toggles_gate_activity_while_stopped() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = app.engine;
        auto &noteEngine = prepareTwoStepSequence(app);

        project.setMonitorMode(Types::MonitorMode::Always);
        engine.clockReset();
        engine.update();
        TEST_ASSERT_FALSE(engine.clockRunning());

        noteEngine.setMonitorStep(0);
        noteEngine.update(0.f);
        TEST_ASSERT_TRUE(noteEngine.gateOutput(0));
        TEST_ASSERT_TRUE(noteEngine.activity());

        noteEngine.setMonitorStep(-1);
        noteEngine.update(0.f);
        TEST_ASSERT_FALSE(noteEngine.gateOutput(0));
        TEST_ASSERT_FALSE(noteEngine.activity());
    
}

void test_06_live_monitoring_uses_active_midi_note_and_clearmidimonitoring_clears_it() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = app.engine;
        auto &noteEngine = prepareSingleStepSequence(app, 0);

        project.setMonitorMode(Types::MonitorMode::Always);
        engine.clockReset();
        engine.update();

        noteEngine.monitorMidi(0, MidiMessage::makeNoteOn(0, 60, 100));
        noteEngine.update(0.f);
        TEST_ASSERT_TRUE(noteEngine.gateOutput(0));
        TEST_ASSERT_TRUE(noteEngine.activity());

        noteEngine.clearMidiMonitoring();
        noteEngine.update(0.f);
        TEST_ASSERT_FALSE(noteEngine.gateOutput(0));
        TEST_ASSERT_FALSE(noteEngine.activity());
    
}

void test_07_step_recorder_writes_note_data_and_advances_to_next_step_on_note_off() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = app.engine;
        auto &noteEngine = prepareTwoStepSequence(app);
        auto &sequence = project.track(0).noteTrack().sequence(0);

        project.setSelectedTrackIndex(0);
        project.setRecordMode(Types::RecordMode::StepRecord);
        engine.setRecording(true);
        noteEngine.update(0.f);

        noteEngine.monitorMidi(0, MidiMessage::makeNoteOn(0, 60, 100));
        TEST_ASSERT_TRUE(sequence.step(0).gate());
        TEST_ASSERT_TRUE((sequence.step(0).length()) == (NoteSequence::Length::Max / 2));

        noteEngine.monitorMidi(1, MidiMessage::makeNoteOff(0, 60));
        TEST_ASSERT_TRUE((noteEngine.currentRecordStep()) == (1));

        // Control change #1 tags tie for the pressed step.
        noteEngine.monitorMidi(2, MidiMessage::makeNoteOn(0, 62, 100));
        noteEngine.monitorMidi(3, MidiMessage::makeControlChange(0, 1, 127));
        TEST_ASSERT_TRUE((sequence.step(1).length()) == (NoteSequence::Length::Max));

        noteEngine.monitorMidi(4, MidiMessage::makeNoteOff(0, 62));
    
}

void test_08_overwrite_mode_clears_previous_step_when_no_note_history_exists() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = app.engine;
        auto &noteEngine = prepareTwoStepSequence(app);
        auto &sequence = project.track(0).noteTrack().sequence(0);

        project.setSelectedTrackIndex(0);
        project.setRecordMode(Types::RecordMode::Overwrite);
        engine.setRecording(true);

        // Prime known non-empty content in step 0.
        sequence.step(0).setGate(true);
        sequence.step(0).setNote(5);

        // First boundary initializes sequence state; second boundary applies overwrite clear
        // to the previous step when no note history was recorded.
        noteEngine.tick(0);
        noteEngine.tick(sequenceDivisorTicks(sequence));

        TEST_ASSERT_FALSE(sequence.step(0).gate());
        TEST_ASSERT_TRUE((sequence.step(0).note()) == (0));
    
}

void test_09_condition_first_triggers_only_on_first_iteration_notfirst_on_later_iterations() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);
        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setLengthVariationRange(0);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setCondition(Types::Condition::First);
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto firstResult = engine.tick(0);
        TEST_ASSERT_TRUE((firstResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto secondResult = engine.tick(divisor);
        TEST_ASSERT_FALSE((secondResult & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));

        step.setCondition(Types::Condition::NotFirst);
        engine.reset();

        auto notFirstStart = engine.tick(0);
        TEST_ASSERT_FALSE((notFirstStart & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto notFirstSecond = engine.tick(divisor);
        TEST_ASSERT_TRUE((notFirstSecond & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_10_condition_pre_follows_previous_condition_state_in_two_step_sequence() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &sequence = project.track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(1);

        auto &step0 = sequence.step(0);
        step0.setGate(true);
        step0.setGateProbability(NoteSequence::GateProbability::Max);
        step0.setLength(0);
        step0.setCondition(Types::Condition::Fill);

        auto &step1 = sequence.step(1);
        step1.setGate(true);
        step1.setGateProbability(NoteSequence::GateProbability::Max);
        step1.setLength(0);
        step1.setCondition(Types::Condition::Pre);

        project.track(0).noteTrack().setFillMode(NoteTrack::FillMode::Condition);
        project.playState().trackState(0).setFillAmount(100);
        project.playState().fillTrack(0, true);
        app.engine.update();
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto step0Result = engine.tick(0);
        TEST_ASSERT_TRUE((step0Result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto step1Result = engine.tick(divisor);
        TEST_ASSERT_TRUE((step1Result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_11_condition_notfill_triggers_when_fill_is_inactive() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &sequence = project.track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);
        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setCondition(Types::Condition::NotFill);

        project.playState().fillTrack(0, false);
        app.engine.update();

        engine.reset();
        auto result = engine.tick(0);
        TEST_ASSERT_TRUE((result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_12_condition_notpre_inverts_previous_condition_state_in_two_step_sequence() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &sequence = project.track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(1);

        auto &step0 = sequence.step(0);
        step0.setGate(true);
        step0.setGateProbability(NoteSequence::GateProbability::Max);
        step0.setLength(0);
        step0.setCondition(Types::Condition::Fill);

        auto &step1 = sequence.step(1);
        step1.setGate(true);
        step1.setGateProbability(NoteSequence::GateProbability::Max);
        step1.setLength(0);
        step1.setCondition(Types::Condition::NotPre);

        project.track(0).noteTrack().setFillMode(NoteTrack::FillMode::Condition);
        project.playState().trackState(0).setFillAmount(100);
        project.playState().fillTrack(0, true);
        app.engine.update();
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto step0Result = engine.tick(0);
        TEST_ASSERT_TRUE((step0Result & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto step1Result = engine.tick(divisor);
        TEST_ASSERT_FALSE((step1Result & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));
    
}

void test_13_condition_loop2_triggers_on_even_iterations() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);
        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setCondition(Types::Condition::Loop2);
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto iter0 = engine.tick(0);
        TEST_ASSERT_TRUE((iter0 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto iter1 = engine.tick(divisor);
        TEST_ASSERT_FALSE((iter1 & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));

        tickRange(engine, divisor + 1, (2 * divisor) - 1);
        auto iter2 = engine.tick(2 * divisor);
        TEST_ASSERT_TRUE((iter2 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_14_condition_notloop2_triggers_on_odd_iterations() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &sequence = app.model.project().track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);
        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setCondition(Types::Condition::NotLoop2);
        engine.reset();

        const uint32_t divisor = sequenceDivisorTicks(sequence);

        auto iter0 = engine.tick(0);
        TEST_ASSERT_FALSE((iter0 & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));

        tickRange(engine, 1, divisor - 1);
        auto iter1 = engine.tick(divisor);
        TEST_ASSERT_TRUE((iter1 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));

        tickRange(engine, divisor + 1, (2 * divisor) - 1);
        auto iter2 = engine.tick(2 * divisor);
        TEST_ASSERT_FALSE((iter2 & TrackEngine::TickResult::GateUpdate) != 0 && engine.gateOutput(0));
    
}

void test_15_linked_track_uses_source_track_link_data_on_tick() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();

        auto &sourceEngine = prepareSingleStepSequence(app, 0);
        auto &linkedEngine = prepareSingleStepSequence(app, 1);

        project.track(1).setLinkTrack(0);
        app.engine.update(); // apply linked track setup

        auto sourceResult = sourceEngine.tick(0);
        TEST_ASSERT_TRUE((sourceResult & TrackEngine::TickResult::GateUpdate) != 0);

        auto linkedResult = linkedEngine.tick(0);
        TEST_ASSERT_TRUE((linkedResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(linkedEngine.gateOutput(0));
    
}

void test_16_slide_enabled_step_updates_cv_gradually_during_update() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &sequence = project.track(0).noteTrack().sequence(0);
        auto &noteTrack = project.track(0).noteTrack();
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);

        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(NoteSequence::Length::Max / 2);
        step.setLengthVariationRange(0);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setGateOffset(0);
        step.setSlide(true);
        step.setNote(24);

        noteTrack.setSlideTime(50);
        engine.reset();

        engine.tick(0);
        float cvBefore = engine.cvOutput(0);

        engine.update(0.05f);
        float cvAfterFirstUpdate = engine.cvOutput(0);
        TEST_ASSERT_TRUE(cvAfterFirstUpdate != cvBefore);

        engine.update(0.05f);
        float cvAfterSecondUpdate = engine.cvOutput(0);
        TEST_ASSERT_TRUE(cvAfterSecondUpdate != cvAfterFirstUpdate);
    
}

void test_17_negative_note_and_length_variation_ranges_execute_variation_inversion_paths() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &sequence = project.track(0).noteTrack().sequence(0);
        auto &engine = app.engine.trackEngine(0).as<NoteTrackEngine>();

        sequence.clearSteps();
        sequence.setFirstStep(0);
        sequence.setLastStep(0);
        auto &step = sequence.step(0);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(4);
        step.setLengthVariationRange(-3);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setGateOffset(0);
        step.setNote(12);
        step.setNoteVariationRange(-5);
        step.setNoteVariationProbability(NoteSequence::NoteVariationProbability::Max);

        // Use a non-chromatic scale to exercise noteFromMidiNote() non-chromatic branch.
        sequence.setScale(14);

        engine.reset();
        auto tickResult = engine.tick(0);
        TEST_ASSERT_TRUE((tickResult & TrackEngine::TickResult::CvUpdate) != 0);

        // Trigger live-monitor conversion path as well.
        project.setMonitorMode(Types::MonitorMode::Always);
        engine.monitorMidi(1, MidiMessage::makeNoteOn(0, 64, 100));
        engine.update(0.f);
        TEST_ASSERT_TRUE(engine.activity());
    
}

void test_18_overdub_mode_records_note_after_deterministic_boundary_progression() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &appEngine = app.engine;
        auto &engine = prepareTwoStepSequence(app);
        auto &sequence = project.track(0).noteTrack().sequence(0);
        const uint32_t divisor = sequenceDivisorTicks(sequence);

        project.setSelectedTrackIndex(0);
        project.setRecordMode(Types::RecordMode::Overdub);
        appEngine.setRecording(true);

        // Prepare a known baseline so we can verify that recording overwrites it.
        sequence.step(0).setGate(false);
        sequence.step(0).setNote(0);
        sequence.step(0).setLength(0);
        sequence.step(1).setGate(false);
        sequence.step(1).setNote(0);
        sequence.step(1).setLength(0);

        engine.tick(0);
        engine.monitorMidi(1, MidiMessage::makeNoteOn(0, 72, 100));

        // Cross two boundaries to avoid first-boundary sequence-state ambiguity
        // and deterministically execute recordStep() with a valid prevStep.
        engine.tick(divisor);
        engine.tick(2 * divisor);

        bool wroteStep0 = sequence.step(0).gate() && sequence.step(0).length() > 0;
        bool wroteStep1 = sequence.step(1).gate() && sequence.step(1).length() > 0;
        TEST_ASSERT_TRUE(wroteStep0 || wroteStep1);

        engine.monitorMidi((2 * divisor) + 1, MidiMessage::makeNoteOff(0, 72));
    
}

void test_19_first_completed_step_live_recording_does_not_underflow_quantization_window() {
    SequencerHarness harness;
    auto &app = harness.app();
    auto &project = app.model.project();
    auto &engine = app.engine;
    auto &noteEngine = prepareTwoStepSequence(app);
    auto &sequence = project.track(0).noteTrack().sequence(0);
    const uint32_t divisor = sequenceDivisorTicks(sequence);

    project.setSelectedTrackIndex(0);
    project.setRecordMode(Types::RecordMode::Overdub);
    engine.setRecording(true);

    sequence.step(0).clear();
    sequence.step(1).clear();

    noteEngine.tick(0);
    noteEngine.monitorMidi(1, MidiMessage::makeNoteOn(0, 72, 100));

    // The first completed step begins at tick 0.  The old unsigned
    // (stepStart - margin) expression wrapped to UINT32_MAX and rejected this
    // note; it must be quantized to step 0 at the very next boundary.
    noteEngine.tick(divisor);

    TEST_ASSERT_TRUE(sequence.step(0).gate());
    TEST_ASSERT_TRUE(sequence.step(0).length() > 0);
    TEST_ASSERT_FALSE(sequence.step(1).gate());

    noteEngine.monitorMidi(divisor + 1, MidiMessage::makeNoteOff(0, 72));
}

void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_playmode_free_advances_on_internal_free_tick_cycle);
    RUN_TEST(test_02_single_step_sequence_reports_finite_zero_progress);
    RUN_TEST(test_03_restart_clears_current_step_after_sequence_was_advanced);
    RUN_TEST(test_04_setmonitorstep_drives_step_recorder_index_only_in_step_record_mode);
    RUN_TEST(test_05_monitor_step_override_toggles_gate_activity_while_stopped);
    RUN_TEST(test_06_live_monitoring_uses_active_midi_note_and_clearmidimonitoring_clears_it);
    RUN_TEST(test_07_step_recorder_writes_note_data_and_advances_to_next_step_on_note_off);
    RUN_TEST(test_08_overwrite_mode_clears_previous_step_when_no_note_history_exists);
    RUN_TEST(test_09_condition_first_triggers_only_on_first_iteration_notfirst_on_later_iterations);
    RUN_TEST(test_10_condition_pre_follows_previous_condition_state_in_two_step_sequence);
    RUN_TEST(test_11_condition_notfill_triggers_when_fill_is_inactive);
    RUN_TEST(test_12_condition_notpre_inverts_previous_condition_state_in_two_step_sequence);
    RUN_TEST(test_13_condition_loop2_triggers_on_even_iterations);
    RUN_TEST(test_14_condition_notloop2_triggers_on_odd_iterations);
    RUN_TEST(test_15_linked_track_uses_source_track_link_data_on_tick);
    RUN_TEST(test_16_slide_enabled_step_updates_cv_gradually_during_update);
    RUN_TEST(test_17_negative_note_and_length_variation_ranges_execute_variation_inversion_paths);
    RUN_TEST(test_18_overdub_mode_records_note_after_deterministic_boundary_progression);
    RUN_TEST(test_19_first_completed_step_live_recording_does_not_underflow_quantization_window);
    return UNITY_END();
}


