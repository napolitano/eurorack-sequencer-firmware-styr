/**
 * @file TestNoteTrackEngineGateOffset.cpp
 * @brief Implements the TestNoteTrackEngineGateOffset component used by the unit test.
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
#include "engine/Groove.h"
#include "model/ProjectVersion.h"

#include "core/Simulator.h"

#include "MemoryReaderWriter.h"
#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#include <cstring>
#include <memory>

// ─── Harness ──────────────────────────────────────────────────────────────────

namespace {

class SequencerHarness {
public:
    SequencerHarness() : _simulator(makeTarget()) { _simulator.reboot(); }
    SequencerApp &app() { return *_app; }

private:
    sim::Target makeTarget() {
        sim::Target t;
        t.create  = [this] () { _app.reset(new SequencerApp()); };
        t.destroy = [this] () { _app.reset(); };
        t.update  = [this] () { _app->update(); };
        return t;
    }
    std::unique_ptr<SequencerApp> _app;
    sim::Simulator _simulator;
};

// Configure track 0 as a single-step sequence.
// gateActive/length/gateOffset are fully controllable.
// All probability fields are set to Max so evaluation is deterministic.
static NoteTrackEngine &prepareSingleStepEngine(
        SequencerApp &app,
        int  gateOffset,
        int  length     = NoteSequence::Length::Max,
        bool gateActive = true,
        int  retrigger  = 0)
{
    auto &seq = app.model.project().track(0).noteTrack().sequence(0);
    seq.clearSteps();
    seq.setFirstStep(0);
    seq.setLastStep(0);

    auto &step = seq.step(0);
    step.setGate(gateActive);
    step.setGateProbability(NoteSequence::GateProbability::Max);
    step.setLength(length);
    step.setLengthVariationRange(0);
    step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
    step.setRetrigger(retrigger);
    step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
    step.setGateOffset(gateOffset);

    auto &eng = app.engine.trackEngine(0).as<NoteTrackEngine>();
    eng.reset();
    return eng;
}

// Tick at which gate-on fires for a given gateOffset value (no swing).
static int32_t gateOnTick(const NoteSequence &seq, int gateOffset) {
    int32_t divisor = seq.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    return (divisor * gateOffset) / (NoteSequence::GateOffset::Max + 1);
}

// Tick duration of the gate (length-based, no variation, no swing).
static uint32_t gateLength(const NoteSequence &seq, int length) {
    uint32_t divisor = seq.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    return (divisor * (length + 1)) / NoteSequence::Length::Range;
}

// Two-step sequence: both steps have gate active.
// step 0 gets offset0, step 1 gets offset1, both use the same length.
static NoteTrackEngine &prepareTwoStepEngine(SequencerApp &app, int offset0, int offset1,
                                              int length = 3) {
    auto &seq = app.model.project().track(0).noteTrack().sequence(0);
    seq.clearSteps();
    seq.setFirstStep(0);
    seq.setLastStep(1);

    for (int i = 0; i <= 1; ++i) {
        auto &step = seq.step(i);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(length);
        step.setLengthVariationRange(0);
        step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::Max);
        step.setRetrigger(0);
        step.setRetriggerProbability(NoteSequence::RetriggerProbability::Max);
        step.setGateOffset(i == 0 ? offset0 : offset1);
    }

    auto &eng = app.engine.trackEngine(0).as<NoteTrackEngine>();
    eng.reset();
    return eng;
}

} // namespace

// ─── Tests ────────────────────────────────────────────────────────────────────

// ── 1. Model: setGateOffset input clamping ─────────────────────────────

void test_01_step_setgateoffset_stores_negative_values_down_to_gateoffset_min() {
        NoteSequence::Step step;
        step.clear();

        step.setGateOffset(-1);
        TEST_ASSERT_TRUE((step.gateOffset()) == (-1));

        step.setGateOffset(NoteSequence::GateOffset::Min); // -7
        TEST_ASSERT_TRUE((step.gateOffset()) == (NoteSequence::GateOffset::Min));

        step.setGateOffset(-99);
        TEST_ASSERT_TRUE((step.gateOffset()) == (NoteSequence::GateOffset::Min));
    
}

void test_02_step_setgateoffset_stores_every_value_in_min_max_without_loss() {
        NoteSequence::Step step;
        step.clear();
        for (int v = NoteSequence::GateOffset::Min; v <= NoteSequence::GateOffset::Max; ++v) {
            step.setGateOffset(v);
            TEST_ASSERT_TRUE((step.gateOffset()) == (v));
        }
    
}

void test_03_step_setgateoffset_clamps_positive_overflow_to_max() {
        NoteSequence::Step step;
        step.clear();

        step.setGateOffset(NoteSequence::GateOffset::Max + 1);
        TEST_ASSERT_TRUE((step.gateOffset()) == (NoteSequence::GateOffset::Max));

        step.setGateOffset(99);
        TEST_ASSERT_TRUE((step.gateOffset()) == (NoteSequence::GateOffset::Max));
    
}

void test_04_step_clear_resets_gateoffset_to_0() {
        NoteSequence::Step step;
        step.setGateOffset(NoteSequence::GateOffset::Max);
        step.clear();
        TEST_ASSERT_TRUE((step.gateOffset()) == (0));
    
}

// ── 2. Model: layer range / default ────────────────────────────────────

void test_05_layerrange_gateoffset_min_equals_gateoffset_min() {
        auto range = NoteSequence::layerRange(NoteSequence::Layer::GateOffset);
        TEST_ASSERT_TRUE((range.min) == (NoteSequence::GateOffset::Min));
    
}

void test_06_layerrange_gateoffset_max_equals_gateoffset_max() {
        auto range = NoteSequence::layerRange(NoteSequence::Layer::GateOffset);
        TEST_ASSERT_TRUE((range.max) == (NoteSequence::GateOffset::Max));
    
}

void test_07_layerdefaultvalue_gateoffset_is_0() {
        TEST_ASSERT_TRUE((NoteSequence::layerDefaultValue(NoteSequence::Layer::GateOffset)) == (0));
    
}

void test_08_setlayervalue_layervalue_round_trip_for_all_valid_gateoffset_values() {
        NoteSequence::Step step;
        step.clear();
        for (int v = NoteSequence::GateOffset::Min; v <= NoteSequence::GateOffset::Max; ++v) {
            step.setLayerValue(NoteSequence::Layer::GateOffset, v);
            TEST_ASSERT_TRUE((step.layerValue(NoteSequence::Layer::GateOffset)) == (v));
        }
    
}

// ── 3. Engine: gate-on timing for every offset value ──────────────────

void test_09_gateoffset_0_gateupdate_fires_at_tick_0_gate_is_high() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0);

        auto r = eng.tick(0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

void test_10_every_gateoffset_value_fires_gateupdate_exactly_at_its_expected_tick() {
        for (int offset = 0; offset <= NoteSequence::GateOffset::Max; ++offset) {
            SequencerHarness h;
            auto &eng    = prepareSingleStepEngine(h.app(), offset);
            uint32_t on  = gateOnTick(eng.sequence(), offset);

            // No gate-on fires before the expected tick
            for (uint32_t t = 0; t < on; ++t) {
                auto r = eng.tick(t);
                TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
            }

            // Gate-on fires exactly at the expected tick
            auto r = eng.tick(on);
            TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
            TEST_ASSERT_TRUE(eng.gateOutput(0));
        }
    
}

// ── 4. Engine: negative input is silently clamped ─────────────────────

void test_11_negative_gateoffset_model_stores_the_signed_value() {
        NoteSequence::Step step;
        step.clear();
        step.setGateOffset(-5);
        TEST_ASSERT_TRUE((step.gateOffset()) == (-5));
    
}

void test_12_negative_gateoffset_on_first_step_starts_at_tick_0() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), -5);

        auto r = eng.tick(0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

void test_13_negative_gateoffset_pre_triggers_only_on_subsequent_loop_passes() {
        SequencerHarness h;
        const int offset = -4;
        auto &eng = prepareSingleStepEngine(h.app(), offset, 0);
        const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        const uint32_t lookAheadOn = divisor + gateOnTick(eng.sequence(), offset);

        // Initial start cannot pre-trigger before tick 0.
        auto r = eng.tick(0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        for (uint32_t t = 1; t < lookAheadOn; ++t) {
            eng.tick(t);
        }

        // Suche im zweiten Durchlauf explizit nach dem ersten Gate-On (Pre-Trigger)
        bool foundPreTrigger = false;
        for (uint32_t t = lookAheadOn; t < divisor * 2; ++t) {
            auto r2 = eng.tick(t);
            if ((r2 & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0)) {
                foundPreTrigger = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(foundPreTrigger);

        // Stelle sicher, dass am Cycle-Boundary kein weiteres Gate-On kommt
        r = eng.tick(divisor);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
    
}

// ── 5. Engine: CV co-arrives with gate-on ─────────────────────────────

void test_14_cv_update_and_gateupdate_arrive_on_the_same_tick_for_gateoffset_0() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0);

        auto r = eng.tick(0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::CvUpdate)   != 0);
    
}

void test_15_cv_update_is_absent_before_gate_on_tick_for_gateoffset_max() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), NoteSequence::GateOffset::Max);
        uint32_t on = gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max);

        for (uint32_t t = 0; t < on; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::CvUpdate) != 0);
        }

        auto r = eng.tick(on);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::CvUpdate) != 0);
    
}

void test_16_no_cv_update_when_gate_is_inactive() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0, NoteSequence::Length::Max, /*gateActive=*/false);

        auto r = eng.tick(0);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::CvUpdate) != 0);
    
}

// ── 6. Engine: gate-off timing ─────────────────────────────────────────
    // Use length=3 (stepLen = divisor*4/8 = 24 ticks) so gate-off lands
    // well before the next step boundary at tick 48 — avoids step-boundary
    // overlap that would cause the next step's gate-on to swallow the test.

void test_17_gate_off_fires_exactly_at_gateontick_steplength_for_gateoffset_0() {
        SequencerHarness h;
        const int  testLen = 3; // stepLen = 24
        auto &eng = prepareSingleStepEngine(h.app(), 0, testLen);
        uint32_t len = gateLength(eng.sequence(), testLen); // 24

        eng.tick(0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        for (uint32_t t = 1; t < len; ++t) { eng.tick(t); }
        TEST_ASSERT_TRUE(eng.gateOutput(0)); // still high at tick 23

        auto r = eng.tick(len);        // tick 24
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(eng.gateOutput(0));
    
}

void test_18_gate_off_fires_at_gateontick_steplength_for_gateoffset_max() {
        SequencerHarness h;
        const int testLen = 3;
        auto &eng = prepareSingleStepEngine(h.app(), NoteSequence::GateOffset::Max, testLen);
        uint32_t on  = gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max); // 42
        uint32_t len = gateLength(eng.sequence(), testLen);                        // 24
        uint32_t off = on + len;                                                   // 66

        for (uint32_t t = 0; t <= on; ++t) eng.tick(t);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        for (uint32_t t = on + 1; t < off; ++t) { eng.tick(t); }
        TEST_ASSERT_TRUE(eng.gateOutput(0)); // still high

        auto r = eng.tick(off);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(eng.gateOutput(0));
    
}

void test_19_minimum_gate_length_length_0_still_fires_a_gate_off_event() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0, 0); // stepLen = 6 ticks
        uint32_t len = gateLength(eng.sequence(), 0);       // 6

        eng.tick(0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        for (uint32_t t = 1; t < len; ++t) { eng.tick(t); }

        auto r = eng.tick(len);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(eng.gateOutput(0));
    
}

// ── 7. Engine: inactive gate ───────────────────────────────────────────

void test_20_inactive_gate_step_no_gateupdate_no_cvupdate_no_gate_output_for_a_full_divisor_window() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0, NoteSequence::Length::Max, /*gateActive=*/false);
        const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);

        for (uint32_t t = 0; t <= divisor; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::CvUpdate)   != 0);
            TEST_ASSERT_FALSE(eng.gateOutput(0));
        }
    
}

// ── 8. Serialization roundtrip ─────────────────────────────────────────

void test_21_step_gateoffset_min_max_survives_serialize_deserialize_at_latest_project_version() {
        uint8_t buf[32];

        for (int v = NoteSequence::GateOffset::Min; v <= NoteSequence::GateOffset::Max; ++v) {
            NoteSequence::Step writeStep;
            writeStep.clear();
            writeStep.setGateOffset(v);

            std::memset(buf, 0, sizeof(buf));
            MemoryWriter memWriter(buf, sizeof(buf));
            VersionedSerializedWriter writer(
                [&memWriter](const void *d, size_t len) { memWriter.write(d, len); },
                ProjectVersion::Latest);
            writeStep.write(writer);

            NoteSequence::Step readStep;
            readStep.clear();
            MemoryReader memReader(buf, sizeof(buf));
            VersionedSerializedReader reader(
                [&memReader](void *d, size_t len) { memReader.read(d, len); },
                ProjectVersion::Latest);
            readStep.read(reader);

            TEST_ASSERT_TRUE((readStep.gateOffset()) == (v));
        }
    
}

// ── 9. Engine reset ────────────────────────────────────────────────────

void test_22_engine_reset_immediately_clears_gate_output_activity_and_cv_output() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 0);

        eng.tick(0);                  // gate fires → gate output high
        TEST_ASSERT_TRUE(eng.gateOutput(0));
        TEST_ASSERT_TRUE(eng.activity());

        eng.reset();
        TEST_ASSERT_FALSE(eng.gateOutput(0));
        TEST_ASSERT_FALSE(eng.activity());
        TEST_ASSERT_TRUE((eng.cvOutput(0)) == (0.0f));
    
}

// ── 10. activity() lifecycle ───────────────────────────────────────────

void test_23_activity_mirrors_gateoutput_throughout_gate_on_off_lifecycle() {
        SequencerHarness h;
        const int testLen = 3;
        auto &eng = prepareSingleStepEngine(h.app(), 0, testLen);
        uint32_t len = gateLength(eng.sequence(), testLen); // 24

        eng.tick(0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
        TEST_ASSERT_TRUE(eng.activity());

        for (uint32_t t = 1; t < len; ++t) eng.tick(t);
        TEST_ASSERT_TRUE(eng.activity()); // still on at tick 23

        eng.tick(len); // gate-off at tick 24
        TEST_ASSERT_FALSE(eng.gateOutput(0));
        TEST_ASSERT_FALSE(eng.activity());
    
}

// ── 11. Multi-step: offsets are per-step, not global ──────────────────

void test_24_two_step_sequence_each_step_fires_at_its_own_gateoffset_relative_to_its_boundary() {
        SequencerHarness h;
        // Step 0: offset=0  → gate-on at tick 0
        // Step 1: offset=Max → gate-on at tick 48 + 42 = 90
        auto &eng = prepareTwoStepEngine(h.app(), 0, NoteSequence::GateOffset::Max);
        const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        uint32_t step1On = divisor + gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max);

        // Step 0 gate fires at tick 0
        auto r0 = eng.tick(0);
        TEST_ASSERT_TRUE((r0 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        // Advance to one tick before step 1's gate-on; gate must be low
        // (step 0's gate-off already fired at its stepLen, no new gate-on until step1On)
        for (uint32_t t = 1; t < step1On; ++t) eng.tick(t);
        TEST_ASSERT_FALSE(eng.gateOutput(0));

        // Step 1 gate fires exactly at step1On
        auto r1 = eng.tick(step1On);
        TEST_ASSERT_TRUE((r1 & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

// ── 12. Divisor scaling ────────────────────────────────────────────────

void test_25_gate_on_timing_scales_proportionally_when_sequence_divisor_is_doubled() {
        SequencerHarness h;
        // Default divisor=12 → 48 ticks/step. Double to 24 → 96 ticks/step.
        h.app().model.project().track(0).noteTrack().sequence(0).setDivisor(24);
        auto &eng = prepareSingleStepEngine(h.app(), 3); // offset=3 → (96*3)/8 = 36
        uint32_t on = gateOnTick(eng.sequence(), 3);     // must equal 36

        for (uint32_t t = 0; t < on; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
        }
        auto r = eng.tick(on);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

// ── 13. CvUpdateMode::Always interaction ───────────────────────────────

void test_26_cvupdatemode_always_cv_updates_at_gateoffset_tick_even_when_gate_is_inactive() {
        SequencerHarness h;
        auto &project = h.app().model.project();
        project.track(0).noteTrack().setCvUpdateMode(NoteTrack::CvUpdateMode::Always);

        auto &eng = prepareSingleStepEngine(h.app(), NoteSequence::GateOffset::Max, NoteSequence::Length::Max, /*gateActive=*/false);
        uint32_t on = gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max);

        for (uint32_t t = 0; t < on; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::CvUpdate) != 0);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0);
        }

        auto r = eng.tick(on);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::CvUpdate) != 0);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE(eng.gateOutput(0));
    
}

// ── 14. Retrigger interaction ──────────────────────────────────────────

void test_27_retrigger_with_gateoffset_first_and_second_pulses_occur_at_expected_delayed_ticks() {
        SequencerHarness h;
        auto &eng = prepareSingleStepEngine(h.app(), 3, NoteSequence::Length::Max, /*gateActive=*/true, /*retrigger=*/1);
        uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        uint32_t on0 = gateOnTick(eng.sequence(), 3);
        uint32_t on1 = on0 + divisor / 2;

        for (uint32_t t = 0; t < on0; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
        }

        auto r = eng.tick(on0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));

        for (uint32_t t = on0 + 1; t < on1; ++t) {
            eng.tick(t);
        }

        r = eng.tick(on1);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

// ── 15. Swing interaction ──────────────────────────────────────────────

void test_28_swing_shifts_gate_event_timing_from_raw_gateoffset_tick() {
        SequencerHarness h;
        auto &project = h.app().model.project();
        project.setSwing(75);

        auto &eng = prepareSingleStepEngine(h.app(), NoteSequence::GateOffset::Max);
        uint32_t rawOn = gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max);
        uint32_t swungOn = Groove::applySwing(rawOn, project.swing());

        for (uint32_t t = 0; t < swungOn; ++t) {
            auto r = eng.tick(t);
            if (t == rawOn) {
                TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0);
            }
        }

        auto r = eng.tick(swungOn);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

// ── 16. Fill mode interaction ──────────────────────────────────────────

void test_29_fill_mode_gates_injects_gate_at_gateoffset_tick_even_when_step_gate_is_off() {
        SequencerHarness h;
        auto &project = h.app().model.project();
        project.playState().fillTrack(0, true);

        auto &eng = prepareSingleStepEngine(h.app(), NoteSequence::GateOffset::Max, NoteSequence::Length::Max, /*gateActive=*/false);
        uint32_t on = gateOnTick(eng.sequence(), NoteSequence::GateOffset::Max);

        for (uint32_t t = 0; t < on; ++t) {
            auto r = eng.tick(t);
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
        }

        auto r = eng.tick(on);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_TRUE(eng.gateOutput(0));
    
}

// ── 17. Mute interaction ───────────────────────────────────────────────

void test_30_muted_track_suppresses_gate_cv_outputs_while_gate_event_still_gets_processed() {
        SequencerHarness h;
        auto &project = h.app().model.project();
        auto &eng = prepareSingleStepEngine(h.app(), 0);

        project.playState().muteTrack(0);
        // Engine update applies immediate mute requests to track state.
        h.app().engine.update();
        TEST_ASSERT_TRUE(project.playState().trackState(0).mute());

        auto r = eng.tick(0);
        TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::CvUpdate) != 0);
        TEST_ASSERT_FALSE(eng.gateOutput(0));
    
}

// ── 18. Negative microtiming boundary and overlap regressions ─────────

void test_31_second_step_negative_offset_pretriggers_exactly_before_its_first_boundary() {
    SequencerHarness h;
    const int offset = -4;
    auto &eng = prepareTwoStepEngine(h.app(), 0, offset, 0);
    const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const uint32_t expected = divisor + gateOnTick(eng.sequence(), offset);

    eng.tick(0);
    for (uint32_t t = 1; t < expected; ++t) {
        auto r = eng.tick(t);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
    }

    auto r = eng.tick(expected);
    TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    r = eng.tick(divisor);
    TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
}

void test_32_forward_wrap_pretriggers_negative_first_step_before_loop_boundary() {
    SequencerHarness h;
    const int offset = -4;
    auto &eng = prepareTwoStepEngine(h.app(), offset, 0, 0);
    const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const uint32_t expected = 2 * divisor + gateOnTick(eng.sequence(), offset);

    eng.tick(0);
    for (uint32_t t = 1; t < expected; ++t) eng.tick(t);

    auto r = eng.tick(expected);
    TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    r = eng.tick(2 * divisor);
    TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
}

void test_33_backward_wrap_pretriggers_negative_last_step_before_loop_boundary() {
    SequencerHarness h;
    const int offset = -4;
    auto &eng = prepareTwoStepEngine(h.app(), 0, offset, 0);
    auto &seq = h.app().model.project().track(0).noteTrack().sequence(0);
    seq.setRunMode(Types::RunMode::Backward);
    eng.reset();

    const uint32_t divisor = seq.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const uint32_t expected = 2 * divisor + gateOnTick(seq, offset);

    eng.tick(0);       // step 1
    eng.tick(divisor); // step 0, reserves wrapped step 1
    for (uint32_t t = divisor + 1; t < expected; ++t) eng.tick(t);

    auto r = eng.tick(expected);
    TEST_ASSERT_TRUE((r & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    r = eng.tick(2 * divisor);
    TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
}

void test_34_stale_gate_off_cannot_cut_short_a_newer_pretriggered_gate() {
    SequencerHarness h;
    auto &eng = prepareTwoStepEngine(h.app(), 4, -2, NoteSequence::Length::Max);

    // step 0: on @ +24, off @ +72; step 1: pretrigger on @ 36, off @ 84.
    // The old step-0 gate-off at 72 must not lower the newer step-1 gate.
    for (uint32_t t = 0; t < 72; ++t) eng.tick(t);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    auto staleOff = eng.tick(72);
    TEST_ASSERT_FALSE((staleOff & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    auto realOff = eng.tick(84);
    TEST_ASSERT_TRUE((realOff & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_FALSE(eng.gateOutput(0));
}

void test_35_reset_measure_is_a_lookahead_barrier_and_clamps_first_postreset_negative_event() {
    SequencerHarness h;
    auto &eng = prepareTwoStepEngine(h.app(), -4, 0, 0);
    auto &seq = h.app().model.project().track(0).noteTrack().sequence(0);
    seq.setResetMeasure(1);
    eng.reset();

    const uint32_t divisor = seq.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const uint32_t resetTick = h.app().engine.measureDivisor();
    const uint32_t pretriggerTick = resetTick + gateOnTick(seq, -4);
    const uint32_t previousBoundary = resetTick - divisor;

    eng.tick(previousBoundary);
    for (uint32_t t = previousBoundary + 1; t < resetTick; ++t) {
        auto r = eng.tick(t);
        if (t >= pretriggerTick) {
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
        }
    }

    auto atReset = eng.tick(resetTick);
    TEST_ASSERT_TRUE((atReset & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));
}

void test_36_pending_pattern_request_blocks_speculative_negative_lookahead() {
    SequencerHarness h;
    auto &eng = prepareTwoStepEngine(h.app(), 0, -4, 0);
    auto &project = h.app().model.project();
    const uint32_t divisor = eng.sequence().divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    const uint32_t pretriggerTick = divisor + gateOnTick(eng.sequence(), -4);

    project.playState().selectTrackPattern(0, 1, PlayState::Synced);
    eng.reset();
    eng.tick(0);

    for (uint32_t t = 1; t < divisor; ++t) {
        auto r = eng.tick(t);
        if (t >= pretriggerTick) {
            TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && eng.gateOutput(0));
        }
    }

    auto boundary = eng.tick(divisor);
    TEST_ASSERT_TRUE((boundary & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));
}

void test_37_linked_track_treats_negative_offset_as_boundary_fallback_not_speculative_prediction() {
    SequencerHarness h;
    auto &project = h.app().model.project();

    auto &sourceSeq = project.track(0).noteTrack().sequence(0);
    sourceSeq.clearSteps();
    sourceSeq.setFirstStep(0);
    sourceSeq.setLastStep(1);
    for (int i = 0; i < 2; ++i) {
        auto &step = sourceSeq.step(i);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setGateOffset(0);
    }

    auto &followerSeq = project.track(1).noteTrack().sequence(0);
    followerSeq.clearSteps();
    followerSeq.setFirstStep(0);
    followerSeq.setLastStep(1);
    followerSeq.setRunMode(Types::RunMode::Backward); // intentionally differs from leader
    for (int i = 0; i < 2; ++i) {
        auto &step = followerSeq.step(i);
        step.setGate(true);
        step.setGateProbability(NoteSequence::GateProbability::Max);
        step.setLength(0);
        step.setGateOffset(i == 1 ? -4 : 0);
    }

    project.track(1).setLinkTrack(0);
    h.app().engine.update();
    auto &source = h.app().engine.trackEngine(0).as<NoteTrackEngine>();
    auto &follower = h.app().engine.trackEngine(1).as<NoteTrackEngine>();
    source.reset();
    follower.reset();

    const uint32_t divisor = sourceSeq.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    source.tick(0);
    follower.tick(0);

    // Follower must not invent the leader's future state from its own Backward run mode.
    for (uint32_t t = 1; t < divisor; ++t) {
        source.tick(t);
        auto r = follower.tick(t);
        TEST_ASSERT_FALSE((r & TrackEngine::TickResult::GateUpdate) != 0 && follower.gateOutput(0));
    }

    source.tick(divisor);
    auto boundary = follower.tick(divisor);
    TEST_ASSERT_TRUE((boundary & TrackEngine::TickResult::GateUpdate) != 0);
}

void test_38_fastest_routed_divisor_preserves_one_tick_minimum_note_gate() {
    SequencerHarness h;
    auto &eng = prepareSingleStepEngine(h.app(), 0, 0);
    auto &seq = h.app().model.project().track(0).noteTrack().sequence(0);
    seq.setDivisor(1);
    eng.reset();

    auto on = eng.tick(0);
    TEST_ASSERT_TRUE((on & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));

    auto off = eng.tick(1);
    TEST_ASSERT_TRUE((off & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_FALSE(eng.gateOutput(0));
}

void test_39_fastest_routed_divisor_limits_retrigger_density_to_resolvable_one_tick_pulses() {
    SequencerHarness h;
    auto &eng = prepareSingleStepEngine(h.app(), 0, NoteSequence::Length::Max, true, 3);
    auto &seq = h.app().model.project().track(0).noteTrack().sequence(0);
    seq.setDivisor(1);
    eng.reset();

    TEST_ASSERT_TRUE((eng.tick(0) & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));
    TEST_ASSERT_TRUE((eng.tick(1) & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_FALSE(eng.gateOutput(0));
    TEST_ASSERT_TRUE((eng.tick(2) & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_TRUE(eng.gateOutput(0));
    TEST_ASSERT_TRUE((eng.tick(3) & TrackEngine::TickResult::GateUpdate) != 0);
    TEST_ASSERT_FALSE(eng.gateOutput(0));
}

void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_step_setgateoffset_stores_negative_values_down_to_gateoffset_min);
    RUN_TEST(test_02_step_setgateoffset_stores_every_value_in_min_max_without_loss);
    RUN_TEST(test_03_step_setgateoffset_clamps_positive_overflow_to_max);
    RUN_TEST(test_04_step_clear_resets_gateoffset_to_0);
    RUN_TEST(test_05_layerrange_gateoffset_min_equals_gateoffset_min);
    RUN_TEST(test_06_layerrange_gateoffset_max_equals_gateoffset_max);
    RUN_TEST(test_07_layerdefaultvalue_gateoffset_is_0);
    RUN_TEST(test_08_setlayervalue_layervalue_round_trip_for_all_valid_gateoffset_values);
    RUN_TEST(test_09_gateoffset_0_gateupdate_fires_at_tick_0_gate_is_high);
    RUN_TEST(test_10_every_gateoffset_value_fires_gateupdate_exactly_at_its_expected_tick);
    RUN_TEST(test_11_negative_gateoffset_model_stores_the_signed_value);
    RUN_TEST(test_12_negative_gateoffset_on_first_step_starts_at_tick_0);
    RUN_TEST(test_13_negative_gateoffset_pre_triggers_only_on_subsequent_loop_passes);
    RUN_TEST(test_14_cv_update_and_gateupdate_arrive_on_the_same_tick_for_gateoffset_0);
    RUN_TEST(test_15_cv_update_is_absent_before_gate_on_tick_for_gateoffset_max);
    RUN_TEST(test_16_no_cv_update_when_gate_is_inactive);
    RUN_TEST(test_17_gate_off_fires_exactly_at_gateontick_steplength_for_gateoffset_0);
    RUN_TEST(test_18_gate_off_fires_at_gateontick_steplength_for_gateoffset_max);
    RUN_TEST(test_19_minimum_gate_length_length_0_still_fires_a_gate_off_event);
    RUN_TEST(test_20_inactive_gate_step_no_gateupdate_no_cvupdate_no_gate_output_for_a_full_divisor_window);
    RUN_TEST(test_21_step_gateoffset_min_max_survives_serialize_deserialize_at_latest_project_version);
    RUN_TEST(test_22_engine_reset_immediately_clears_gate_output_activity_and_cv_output);
    RUN_TEST(test_23_activity_mirrors_gateoutput_throughout_gate_on_off_lifecycle);
    RUN_TEST(test_24_two_step_sequence_each_step_fires_at_its_own_gateoffset_relative_to_its_boundary);
    RUN_TEST(test_25_gate_on_timing_scales_proportionally_when_sequence_divisor_is_doubled);
    RUN_TEST(test_26_cvupdatemode_always_cv_updates_at_gateoffset_tick_even_when_gate_is_inactive);
    RUN_TEST(test_27_retrigger_with_gateoffset_first_and_second_pulses_occur_at_expected_delayed_ticks);
    RUN_TEST(test_28_swing_shifts_gate_event_timing_from_raw_gateoffset_tick);
    RUN_TEST(test_29_fill_mode_gates_injects_gate_at_gateoffset_tick_even_when_step_gate_is_off);
    RUN_TEST(test_30_muted_track_suppresses_gate_cv_outputs_while_gate_event_still_gets_processed);
    RUN_TEST(test_31_second_step_negative_offset_pretriggers_exactly_before_its_first_boundary);
    RUN_TEST(test_32_forward_wrap_pretriggers_negative_first_step_before_loop_boundary);
    RUN_TEST(test_33_backward_wrap_pretriggers_negative_last_step_before_loop_boundary);
    RUN_TEST(test_34_stale_gate_off_cannot_cut_short_a_newer_pretriggered_gate);
    RUN_TEST(test_35_reset_measure_is_a_lookahead_barrier_and_clamps_first_postreset_negative_event);
    RUN_TEST(test_36_pending_pattern_request_blocks_speculative_negative_lookahead);
    RUN_TEST(test_37_linked_track_treats_negative_offset_as_boundary_fallback_not_speculative_prediction);
    RUN_TEST(test_38_fastest_routed_divisor_preserves_one_tick_minimum_note_gate);
    RUN_TEST(test_39_fastest_routed_divisor_limits_retrigger_density_to_resolvable_one_tick_pulses);
    return UNITY_END();
}








