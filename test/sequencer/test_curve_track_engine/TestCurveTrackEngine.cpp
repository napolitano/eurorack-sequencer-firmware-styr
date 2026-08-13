/**
 * @file TestCurveTrackEngine.cpp
 * @brief Implements the TestCurveTrackEngine component used by the unit test.
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

static CurveTrackEngine &configureCurveTrack(SequencerApp &app, int trackIndex) {
    auto &project = app.model.project();
    project.setTrackMode(trackIndex, Track::TrackMode::Curve);
    app.engine.update();

    auto &track = project.track(trackIndex);
    auto &curveTrack = track.curveTrack();
    auto &sequence = curveTrack.sequence(0);

    sequence.clearSteps();
    sequence.setFirstStep(0);
    sequence.setLastStep(0);
    sequence.setDivisor(12);
    sequence.setResetMeasure(0);

    auto &step = sequence.step(0);
    step.setShape(0);
    step.setShapeVariation(1);
    step.setShapeVariationProbability(0);
    step.setMin(0);
    step.setMax(CurveSequence::Max::Max);
    step.setGate(0b0001);
    step.setGateProbability(CurveSequence::GateProbability::Max);

    curveTrack.setPlayMode(Types::PlayMode::Aligned);
    curveTrack.setMuteMode(CurveTrack::MuteMode::LastValue);
    curveTrack.setFillMode(CurveTrack::FillMode::None);
    curveTrack.setSlideTime(0);
    curveTrack.setOffset(0);
    curveTrack.setRotate(0);
    curveTrack.setGateProbabilityBias(0);
    curveTrack.setShapeProbabilityBias(0);

    auto &engine = app.engine.trackEngine(trackIndex).as<CurveTrackEngine>();
    engine.reset();
    return engine;
}

static uint32_t sequenceDivisorTicks(const CurveSequence &sequence) {
    return sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
}

} // namespace

void test_01_aligned_play_mode_triggers_step_and_produces_gate_on_off_events() {
        SequencerHarness harness;
        auto &engine = configureCurveTrack(harness.app(), 0);

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
        const uint32_t gateOffTick = divisor / 8;

        auto tick0 = engine.tick(0);
        TEST_ASSERT_TRUE((tick0 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
        TEST_ASSERT_TRUE((engine.currentStep()) == (0));

        auto off = engine.tick(gateOffTick);
        TEST_ASSERT_TRUE((off & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(engine.gateOutput(0));
    
}

void test_02_single_step_sequence_reports_finite_zero_progress() {
        SequencerHarness harness;
        auto &engine = configureCurveTrack(harness.app(), 0);

        engine.tick(0);
        TEST_ASSERT_TRUE((engine.currentStep()) == (0));
        TEST_ASSERT_TRUE((engine.sequenceProgress()) == (0.f));
    
}

void test_03_play_mode_free_advances_the_step_and_playmode_last_input_clamps_to_free() {
        SequencerHarness harness;
        auto &freeEngine = configureCurveTrack(harness.app(), 0);
        auto &curveTrack = harness.app().model.project().track(0).curveTrack();

        curveTrack.setPlayMode(Types::PlayMode::Free);
        freeEngine.reset();
        freeEngine.tick(0);
        TEST_ASSERT_TRUE((freeEngine.currentStep()) == (0));

        // PlayMode::Last is a sentinel; model setters clamp it to the last valid mode (Free).
        curveTrack.setPlayMode(Types::PlayMode::Last);
        freeEngine.reset();
        freeEngine.tick(0);
        TEST_ASSERT_TRUE((freeEngine.currentStep()) == (0));
    
}

void test_04_mute_state_suppresses_gate_output_unless_fill_is_active() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &playState = app.model.project().playState();

        playState.muteTrack(0, PlayState::Immediate);
        app.engine.update();
        auto mutedResult = engine.tick(0);
        TEST_ASSERT_TRUE((mutedResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(engine.gateOutput(0));

        engine.reset();
        playState.trackState(0).setFillAmount(100);
        playState.fillTrack(0, true);
        app.engine.update();
        auto fillResult = engine.tick(0);
        TEST_ASSERT_TRUE((fillResult & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(engine.gateOutput(0));
    
}

void test_05_mute_modes_zero_min_and_max_drive_dedicated_cv_targets() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &project = app.model.project();
        auto &playState = project.playState();
        auto &curveTrack = project.track(0).curveTrack();
        auto &sequence = curveTrack.sequence(0);
        const auto &range = Types::voltageRangeInfo(sequence.range());

        playState.muteTrack(0, PlayState::Immediate);
        app.engine.update();

        curveTrack.setMuteMode(CurveTrack::MuteMode::Zero);
        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (0.f));

        curveTrack.setMuteMode(CurveTrack::MuteMode::Min);
        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (range.lo));

        curveTrack.setMuteMode(CurveTrack::MuteMode::Max);
        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        TEST_ASSERT_TRUE((engine.cvOutput(0)) == (range.hi));
    
}

void test_06_fill_mode_nextpattern_evaluates_the_next_pattern_when_fill_is_active() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &project = app.model.project();
        auto &playState = project.playState();
        auto &curveTrack = project.track(0).curveTrack();

        auto &baseStep = curveTrack.sequence(0).step(0);
        baseStep.setMinNormalized(0.f);
        baseStep.setMaxNormalized(0.f);

        auto &fillStep = curveTrack.sequence(1).step(0);
        fillStep.setMinNormalized(1.f);
        fillStep.setMaxNormalized(1.f);

        curveTrack.setFillMode(CurveTrack::FillMode::NextPattern);

        playState.trackState(0).setFillAmount(0);
        playState.fillTrack(0, false);
        app.engine.update();

        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        float cvWithoutFill = engine.cvOutput(0);

        playState.trackState(0).setFillAmount(100);
        playState.fillTrack(0, true);
        app.engine.update();

        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        float cvWithFill = engine.cvOutput(0);

        TEST_ASSERT_TRUE(cvWithFill != cvWithoutFill);
    
}

void test_07_monitor_step_override_selects_min_and_max_value_when_engine_is_stopped() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &step = app.model.project().track(0).curveTrack().sequence(0).step(0);

        step.setMinNormalized(0.f);
        step.setMaxNormalized(1.f);

        app.engine.setRecording(false);

        engine.setMonitorStep(0);
        engine.setMonitorStepLevel(CurveTrackEngine::MonitorLevel::Min);
        engine.update(0.f);
        float minCv = engine.cvOutput(0);

        engine.setMonitorStepLevel(CurveTrackEngine::MonitorLevel::Max);
        engine.update(0.f);
        float maxCv = engine.cvOutput(0);

        TEST_ASSERT_TRUE(maxCv > minCv);
    
}

void test_08_recording_path_reads_selected_cv_input_channel() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = configureCurveTrack(app, 0);

        project.setSelectedTrackIndex(0);
        app.engine.setRecording(true);

        project.setCurveCvInput(Types::CurveCvInput::Cv1);
        harness.setCvInput(0, 4.f);
        app.update();
        float cv1Value = engine.cvOutput(0);

        project.setCurveCvInput(Types::CurveCvInput::Cv2);
        harness.setCvInput(1, 0.f);
        app.update();
        float cv2Value = engine.cvOutput(0);

        TEST_ASSERT_TRUE(cv1Value != cv2Value);
    
}

void test_09_linked_curve_track_follows_leader_link_data() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();

        auto &leader = configureCurveTrack(app, 0);
        auto &follower = configureCurveTrack(app, 1);

        project.track(1).setLinkTrack(0);
        app.engine.update();

        leader.reset();
        follower.reset();

        leader.tick(0);
        follower.tick(0);

        TEST_ASSERT_TRUE((leader.currentStep()) == (0));
        TEST_ASSERT_TRUE((follower.currentStep()) == (leader.currentStep()));
        TEST_ASSERT_TRUE(follower.linkedTrackEngine() != nullptr);
    
}

void test_10_pattern_switch_updates_active_curve_sequence_pointer() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &playState = project.playState();

        auto &engine = configureCurveTrack(app, 0);
        auto &curveTrack = project.track(0).curveTrack();

        TEST_ASSERT_TRUE(engine.isActiveSequence(curveTrack.sequence(0)));

        playState.selectTrackPattern(0, 1, PlayState::Immediate);
        app.engine.update();

        TEST_ASSERT_TRUE(engine.isActiveSequence(curveTrack.sequence(1)));
        TEST_ASSERT_FALSE(engine.isActiveSequence(curveTrack.sequence(0)));
    
}

void test_11_restart_resets_current_step_and_clears_intra_step_fraction() {
        SequencerHarness harness;
        auto &engine = configureCurveTrack(harness.app(), 0);

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());

        engine.tick(0);
        engine.tick(divisor / 4);
        TEST_ASSERT_TRUE((engine.currentStep()) == (0));
        TEST_ASSERT_TRUE(engine.currentStepFraction() > 0.f);

        engine.restart();
        TEST_ASSERT_TRUE((engine.currentStep()) == (-1));
        TEST_ASSERT_TRUE((engine.currentStepFraction()) == (0.f));
    
}

void test_12_slide_time_applies_smoothing_instead_of_jumping_directly_to_target_cv() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &curveTrack = app.model.project().track(0).curveTrack();
        auto &step = curveTrack.sequence(0).step(0);

        // Start from a low target and apply update once.
        step.setMinNormalized(0.f);
        step.setMaxNormalized(0.f);
        curveTrack.setSlideTime(0);
        engine.reset();
        engine.tick(0);
        engine.update(0.f);
        float lowCv = engine.cvOutput(0);

        // Move target high and enable slide, then verify the transition is gradual.
        step.setMinNormalized(1.f);
        step.setMaxNormalized(1.f);
        curveTrack.setSlideTime(100);
        engine.tick(0);
        engine.update(0.001f);
        float smoothedCv = engine.cvOutput(0);

        TEST_ASSERT_TRUE(smoothedCv > lowCv);
        TEST_ASSERT_TRUE(smoothedCv < 5.f);
    
}

void test_13_fill_invert_changes_evaluated_curve_output_compared_to_normal_fill_off_mode() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &project = app.model.project();
        auto &playState = project.playState();
        auto &curveTrack = project.track(0).curveTrack();
        auto &step = curveTrack.sequence(0).step(0);

        step.setMinNormalized(0.f);
        step.setMaxNormalized(1.f);

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
        const uint32_t sampleTick = divisor / 4;

        curveTrack.setFillMode(CurveTrack::FillMode::Invert);

        playState.trackState(0).setFillAmount(0);
        playState.fillTrack(0, false);
        app.engine.update();
        engine.reset();
        engine.tick(0);
        engine.tick(sampleTick);
        engine.update(0.f);
        float normalCv = engine.cvOutput(0);

        playState.trackState(0).setFillAmount(100);
        playState.fillTrack(0, true);
        app.engine.update();
        engine.reset();
        engine.tick(0);
        engine.tick(sampleTick);
        engine.update(0.f);
        float invertCv = engine.cvOutput(0);

        TEST_ASSERT_TRUE(invertCv != normalCv);
    
}

void test_14_recording_with_unsupported_curve_cv_input_ignores_external_adc_value() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = configureCurveTrack(app, 0);

        project.setSelectedTrackIndex(0);
        app.engine.setRecording(true);
        project.setCurveCvInput(Types::CurveCvInput::Last);

        harness.setCvInput(0, 4.f);
        app.update();
        float firstValue = engine.cvOutput(0);

        harness.setCvInput(0, 0.f);
        app.update();
        float secondValue = engine.cvOutput(0);

        TEST_ASSERT_TRUE((firstValue) == (secondValue));
    
}

void test_15_recording_over_one_full_divisor_updates_step_shape_or_range_via_recorder_match() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();
        auto &engine = configureCurveTrack(app, 0);
        auto &step = project.track(0).curveTrack().sequence(0).step(0);

        step.setShape(3);
        step.setMin(64);
        step.setMax(192);

        const int initialShape = step.shape();
        const int initialMin = step.min();
        const int initialMax = step.max();

        project.setSelectedTrackIndex(0);
        app.engine.setRecording(true);
        project.setCurveCvInput(Types::CurveCvInput::Cv1);

        // Prime CV input cache used by updateRecordValue().
        harness.setCvInput(0, 4.f);
        app.update();

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
        for (uint32_t tick = 0; tick <= divisor; ++tick) {
            engine.tick(tick);
        }

        const bool stepChanged =
            step.shape() != initialShape ||
            step.min() != initialMin ||
            step.max() != initialMax;

        TEST_ASSERT_TRUE(stepChanged);
    
}

void test_16_tick_before_first_boundary_keeps_sequence_step_inactive() {
        SequencerHarness harness;
        auto &engine = configureCurveTrack(harness.app(), 0);

        engine.reset();
        auto result = engine.tick(1);

        TEST_ASSERT_TRUE((result) == (TrackEngine::TickResult::NoUpdate));
        TEST_ASSERT_TRUE((engine.currentStep()) == (-1));
    
}

void test_17_shape_probability_bias_forces_shape_variation_branch() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &project = app.model.project();
        auto &curveTrack = project.track(0).curveTrack();
        auto &step = curveTrack.sequence(0).step(0);

        step.setShape(0);
        step.setShapeVariation(1);
        step.setShapeVariationProbability(0);
        curveTrack.setShapeProbabilityBias(8); // clamp to always-on variation

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
        const uint32_t sampleTick = divisor / 4;

        engine.reset();
        engine.tick(0);
        engine.tick(sampleTick);
        engine.update(0.f);

        TEST_ASSERT_TRUE(engine.cvOutput(0) > 0.f);
    
}

void test_18_fill_active_with_zero_amount_keeps_fill_mode_at_none() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &engine = configureCurveTrack(app, 0);
        auto &project = app.model.project();
        auto &playState = project.playState();
        auto &curveTrack = project.track(0).curveTrack();
        auto &step = curveTrack.sequence(0).step(0);

        step.setMinNormalized(0.f);
        step.setMaxNormalized(1.f);

        const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
        const uint32_t sampleTick = divisor / 4;

        curveTrack.setFillMode(CurveTrack::FillMode::Variation);

        playState.trackState(0).setFillAmount(0);
        playState.fillTrack(0, false);
        app.engine.update();

        engine.reset();
        engine.tick(0);
        engine.tick(sampleTick);
        engine.update(0.f);
        float cvWithoutFill = engine.cvOutput(0);

        playState.trackState(0).setFillAmount(0);
        playState.fillTrack(0, true);
        app.engine.update();

        engine.reset();
        engine.tick(0);
        engine.tick(sampleTick);
        engine.update(0.f);
        float cvWithZeroFill = engine.cvOutput(0);

        TEST_ASSERT_TRUE((cvWithZeroFill) == (cvWithoutFill));
    
}

void test_19_linked_follower_ignores_non_boundary_relative_tick() {
        SequencerHarness harness;
        auto &app = harness.app();
        auto &project = app.model.project();

        auto &leader = configureCurveTrack(app, 0);
        auto &follower = configureCurveTrack(app, 1);

        project.track(1).setLinkTrack(0);
        app.engine.update();

        leader.reset();
        follower.reset();

        // Non-boundary tick keeps linked step inactive.
        leader.tick(1);
        auto followerResult = follower.tick(1);

        TEST_ASSERT_TRUE((follower.currentStep()) == (-1));
        TEST_ASSERT_TRUE((followerResult) == (TrackEngine::TickResult::NoUpdate));
    
}

void test_20_free_mode_divisor_change_keeps_local_phase_instead_of_snapping_to_absolute_grid() {
    SequencerHarness harness;
    auto &app = harness.app();
    auto &engine = configureCurveTrack(app, 0);
    auto &curveTrack = app.model.project().track(0).curveTrack();
    auto &sequence = curveTrack.sequence(0);

    sequence.setLastStep(1);
    curveTrack.setPlayMode(Types::PlayMode::Free);
    engine.reset();

    engine.tick(0);
    TEST_ASSERT_TRUE((engine.currentStep()) == (0));

    // With the original 48-tick divisor, advance well beyond the future
    // 32-tick divisor before changing it.  Free mode should finish its local
    // phase and trigger the next step on the following tick, rather than wait
    // for the next absolute 32-tick grid line at tick 64.
    for (uint32_t t = 1; t < 40; ++t) engine.tick(t);
    sequence.setDivisor(8); // 8 * (192/48) = 32 engine ticks

    engine.tick(40);
    TEST_ASSERT_TRUE((engine.currentStep()) == (0));
    engine.tick(41);
    TEST_ASSERT_TRUE((engine.currentStep()) == (1));
}

void test_21_fill_nextpattern_uses_next_pattern_gate_data_not_base_pattern_gate_data() {
    SequencerHarness harness;
    auto &app = harness.app();
    auto &engine = configureCurveTrack(app, 0);
    auto &project = app.model.project();
    auto &curveTrack = project.track(0).curveTrack();

    curveTrack.sequence(0).step(0).setGate(0);
    curveTrack.sequence(1).step(0).setGate(0b0001);
    curveTrack.sequence(1).step(0).setGateProbability(CurveSequence::GateProbability::Max);
    curveTrack.setFillMode(CurveTrack::FillMode::NextPattern);
    project.playState().trackState(0).setFillAmount(100);
    project.playState().fillTrack(0, true);
    app.engine.update();

    engine.reset();
    auto result = engine.tick(0);
    TEST_ASSERT_TRUE((result & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(engine.gateOutput(0));
}

void test_22_fill_nextpattern_uses_next_pattern_shape_variation_probability() {
    SequencerHarness harness;
    auto &app = harness.app();
    auto &engine = configureCurveTrack(app, 0);
    auto &project = app.model.project();
    auto &curveTrack = project.track(0).curveTrack();

    auto &base = curveTrack.sequence(0).step(0);
    base.setShape(0);
    base.setShapeVariation(0);
    base.setShapeVariationProbability(0);

    auto &fill = curveTrack.sequence(1).step(0);
    fill.setShape(0);
    fill.setShapeVariation(1);
    fill.setShapeVariationProbability(8);
    fill.setMinNormalized(0.f);
    fill.setMaxNormalized(1.f);

    curveTrack.setFillMode(CurveTrack::FillMode::NextPattern);
    project.playState().trackState(0).setFillAmount(100);
    project.playState().fillTrack(0, true);
    app.engine.update();

    const uint32_t divisor = sequenceDivisorTicks(engine.sequence());
    engine.reset();
    engine.tick(0);
    engine.tick(divisor / 4);
    engine.update(0.f);

    TEST_ASSERT_TRUE(engine.cvOutput(0) > 0.f);
}

void test_23_fastest_routed_divisor_preserves_one_tick_curve_gate_pulse() {
    SequencerHarness harness;
    auto &app = harness.app();
    auto &engine = configureCurveTrack(app, 0);
    auto &sequence = app.model.project().track(0).curveTrack().sequence(0);
    sequence.setDivisor(1);
    engine.reset();

    auto on = engine.tick(0);
    TEST_ASSERT_TRUE((on & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(engine.gateOutput(0));

    auto off = engine.tick(1);
    TEST_ASSERT_TRUE((off & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_FALSE(engine.gateOutput(0));
}

void test_24_curve_sequence_gate_only_edit_is_reported_as_edited() {
    CurveSequence sequence;
    sequence.clear();
    TEST_ASSERT_FALSE(sequence.isEdited());

    sequence.step(0).setGate(1);
    TEST_ASSERT_TRUE(sequence.isEdited());
}

void test_25_curve_sequence_gate_probability_only_edit_is_reported_as_edited() {
    CurveSequence sequence;
    sequence.clear();
    TEST_ASSERT_FALSE(sequence.isEdited());

    sequence.step(0).setGateProbability(CurveSequence::GateProbability::Max - 1);
    TEST_ASSERT_TRUE(sequence.isEdited());
}

void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_aligned_play_mode_triggers_step_and_produces_gate_on_off_events);
    RUN_TEST(test_02_single_step_sequence_reports_finite_zero_progress);
    RUN_TEST(test_03_play_mode_free_advances_the_step_and_playmode_last_input_clamps_to_free);
    RUN_TEST(test_04_mute_state_suppresses_gate_output_unless_fill_is_active);
    RUN_TEST(test_05_mute_modes_zero_min_and_max_drive_dedicated_cv_targets);
    RUN_TEST(test_06_fill_mode_nextpattern_evaluates_the_next_pattern_when_fill_is_active);
    RUN_TEST(test_07_monitor_step_override_selects_min_and_max_value_when_engine_is_stopped);
    RUN_TEST(test_08_recording_path_reads_selected_cv_input_channel);
    RUN_TEST(test_09_linked_curve_track_follows_leader_link_data);
    RUN_TEST(test_10_pattern_switch_updates_active_curve_sequence_pointer);
    RUN_TEST(test_11_restart_resets_current_step_and_clears_intra_step_fraction);
    RUN_TEST(test_12_slide_time_applies_smoothing_instead_of_jumping_directly_to_target_cv);
    RUN_TEST(test_13_fill_invert_changes_evaluated_curve_output_compared_to_normal_fill_off_mode);
    RUN_TEST(test_14_recording_with_unsupported_curve_cv_input_ignores_external_adc_value);
    RUN_TEST(test_15_recording_over_one_full_divisor_updates_step_shape_or_range_via_recorder_match);
    RUN_TEST(test_16_tick_before_first_boundary_keeps_sequence_step_inactive);
    RUN_TEST(test_17_shape_probability_bias_forces_shape_variation_branch);
    RUN_TEST(test_18_fill_active_with_zero_amount_keeps_fill_mode_at_none);
    RUN_TEST(test_19_linked_follower_ignores_non_boundary_relative_tick);
    RUN_TEST(test_20_free_mode_divisor_change_keeps_local_phase_instead_of_snapping_to_absolute_grid);
    RUN_TEST(test_21_fill_nextpattern_uses_next_pattern_gate_data_not_base_pattern_gate_data);
    RUN_TEST(test_22_fill_nextpattern_uses_next_pattern_shape_variation_probability);
    RUN_TEST(test_23_fastest_routed_divisor_preserves_one_tick_curve_gate_pulse);
    RUN_TEST(test_24_curve_sequence_gate_only_edit_is_reported_as_edited);
    RUN_TEST(test_25_curve_sequence_gate_probability_only_edit_is_reported_as_edited);
    return UNITY_END();
}

