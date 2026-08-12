/**
 * @file TestSequenceState.cpp
 * @brief Implements the TestSequenceState component used by the unit test.
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

#include "engine/SequenceState.h"

namespace {

static Random seeded(uint32_t seed) {
    return Random(seed);
}

static uint32_t seedSelecting(uint32_t range, uint32_t value) {
    for (uint32_t seed = 0; seed < 65536; ++seed) {
        Random probe(seed);
        if (probe.nextRange(range) == value) {
            return seed;
        }
    }
    return uint32_t(-1);
}

} // namespace

void test_01_reset_initializes_step_history_direction_and_iteration() {
        SequenceState state;
        state.reset();

        TEST_ASSERT_TRUE((state.step()) == (-1));
        TEST_ASSERT_TRUE((state.prevStep()) == (-1));
        TEST_ASSERT_TRUE((state.direction()) == (1));
        TEST_ASSERT_TRUE((state.iteration()) == (uint32_t(0)));
    
}

void test_02_advancefree_forward_wraps_to_first_step_and_increments_iteration() {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (2));
        TEST_ASSERT_TRUE((state.prevStep()) == (-1));
        TEST_ASSERT_TRUE((state.iteration()) == (uint32_t(0)));

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (3));
        TEST_ASSERT_TRUE((state.prevStep()) == (2));

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (4));
        TEST_ASSERT_TRUE((state.prevStep()) == (3));

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (2));
        TEST_ASSERT_TRUE((state.prevStep()) == (4));
        TEST_ASSERT_TRUE((state.iteration()) == (uint32_t(1)));
    
}

void test_03_advancefree_backward_wraps_to_last_step_and_increments_iteration() {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (4));
        TEST_ASSERT_TRUE((state.prevStep()) == (-1));

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (3));
        TEST_ASSERT_TRUE((state.prevStep()) == (4));

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (2));
        TEST_ASSERT_TRUE((state.prevStep()) == (3));

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        TEST_ASSERT_TRUE((state.step()) == (4));
        TEST_ASSERT_TRUE((state.prevStep()) == (2));
        TEST_ASSERT_TRUE((state.iteration()) == (uint32_t(1)));
    
}

void test_04_advancefree_pendulum_holds_endpoints_while_pingpong_bounces_immediately() {
        SequenceState pendulum;
        SequenceState pingPong;
        Random pendulumRng = seeded(0);
        Random pingPongRng = seeded(0);
        pendulum.reset();
        pingPong.reset();

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (2));
        TEST_ASSERT_TRUE((pendulum.direction()) == (1));

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (2));
        TEST_ASSERT_TRUE((pendulum.direction()) == (-1));

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (1));
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (0));
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (0));
        TEST_ASSERT_TRUE((pendulum.direction()) == (1));
        TEST_ASSERT_TRUE((pendulum.iteration()) == (uint32_t(1)));

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (2));
        TEST_ASSERT_TRUE((pingPong.direction()) == (1));

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (1));
        TEST_ASSERT_TRUE((pingPong.direction()) == (-1));

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (0));
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (1));
        TEST_ASSERT_TRUE((pingPong.direction()) == (1));
        TEST_ASSERT_TRUE((pingPong.iteration()) == (uint32_t(1)));
    
}

void test_05_advancefree_clamps_out_of_range_current_step_before_advancing() {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Backward, 0, 5, rng);
        TEST_ASSERT_TRUE((state.step()) == (5));

        state.advanceFree(Types::RunMode::Forward, 2, 3, rng);
        TEST_ASSERT_TRUE((state.prevStep()) == (5));
        TEST_ASSERT_TRUE((state.step()) == (2));
        TEST_ASSERT_TRUE((state.iteration()) == (uint32_t(1)));
    
}

void test_06_advancefree_random_follows_random_and_randomwalk_wraps_at_both_boundaries() {
        SequenceState randomState;
        Random randomRng = seeded(0);
        Random expectedRandomRng = seeded(0);
        randomState.reset();

        const int expectedRandomStep = int(expectedRandomRng.nextRange(3)) + 3;
        randomState.advanceFree(Types::RunMode::Random, 3, 5, randomRng);
        TEST_ASSERT_TRUE((randomState.step()) == (expectedRandomStep));
        TEST_ASSERT_TRUE((randomState.prevStep()) == (-1));

        // Put the state on the left boundary without consuming the RNG, then
        // choose a seed whose first binary draw selects the negative direction.
        const uint32_t leftSeed = seedSelecting(2, 0);
        TEST_ASSERT_TRUE(leftSeed != uint32_t(-1));
        SequenceState walkLeftWrap;
        Random leftWrapRng = seeded(leftSeed);
        walkLeftWrap.reset();
        walkLeftWrap.advanceFree(Types::RunMode::Forward, 0, 1, leftWrapRng);
        TEST_ASSERT_TRUE((walkLeftWrap.step()) == (0));
        walkLeftWrap.advanceFree(Types::RunMode::RandomWalk, 0, 1, leftWrapRng);
        TEST_ASSERT_TRUE((walkLeftWrap.step()) == (1));

        // Likewise exercise the right-boundary wrap explicitly with a seed
        // whose first binary draw selects the positive direction.
        const uint32_t rightSeed = seedSelecting(2, 1);
        TEST_ASSERT_TRUE(rightSeed != uint32_t(-1));
        SequenceState walkRightWrap;
        Random rightWrapRng = seeded(rightSeed);
        walkRightWrap.reset();
        walkRightWrap.advanceFree(Types::RunMode::Backward, 0, 1, rightWrapRng);
        TEST_ASSERT_TRUE((walkRightWrap.step()) == (1));
        walkRightWrap.advanceFree(Types::RunMode::RandomWalk, 0, 1, rightWrapRng);
        TEST_ASSERT_TRUE((walkRightWrap.step()) == (0));
    
}

void test_07_advancefree_run_mode_last_preserves_the_current_step() {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Last, 0, 3, rng);
        TEST_ASSERT_TRUE((state.step()) == (-1));
        TEST_ASSERT_TRUE((state.prevStep()) == (-1));

        state.advanceFree(Types::RunMode::Forward, 0, 3, rng);
        TEST_ASSERT_TRUE((state.step()) == (0));
        state.advanceFree(Types::RunMode::Last, 0, 3, rng);
        TEST_ASSERT_TRUE((state.prevStep()) == (0));
        TEST_ASSERT_TRUE((state.step()) == (0));
    
}

void test_08_advancealigned_forward_and_backward_map_absolute_step_and_iteration_deterministically() {
        SequenceState forward;
        SequenceState backward;
        Random rng0 = seeded(0);
        Random rng1 = seeded(0);
        forward.reset();
        backward.reset();

        forward.advanceAligned(5, Types::RunMode::Forward, 2, 4, rng0);
        TEST_ASSERT_TRUE((forward.step()) == (4));
        TEST_ASSERT_TRUE((forward.prevStep()) == (-1));
        TEST_ASSERT_TRUE((forward.iteration()) == (uint32_t(1)));

        backward.advanceAligned(4, Types::RunMode::Backward, 2, 4, rng1);
        TEST_ASSERT_TRUE((backward.step()) == (3));
        TEST_ASSERT_TRUE((backward.prevStep()) == (-1));
        TEST_ASSERT_TRUE((backward.iteration()) == (uint32_t(1)));
    
}

void test_09_single_step_pingpong_remains_stable_in_free_and_aligned_modes() {
        SequenceState freeState;
        SequenceState alignedState;
        Random freeRng = seeded(0);
        Random alignedRng = seeded(0);
        freeState.reset();
        alignedState.reset();

        freeState.advanceFree(Types::RunMode::PingPong, 4, 4, freeRng);
        TEST_ASSERT_TRUE((freeState.step()) == (4));
        TEST_ASSERT_TRUE((freeState.iteration()) == (uint32_t(0)));

        freeState.advanceFree(Types::RunMode::PingPong, 4, 4, freeRng);
        TEST_ASSERT_TRUE((freeState.step()) == (4));
        TEST_ASSERT_TRUE((freeState.iteration()) == (uint32_t(1)));

        alignedState.advanceAligned(0, Types::RunMode::PingPong, 4, 4, alignedRng);
        TEST_ASSERT_TRUE((alignedState.step()) == (4));
        TEST_ASSERT_TRUE((alignedState.iteration()) == (uint32_t(0)));

        alignedState.advanceAligned(7, Types::RunMode::PingPong, 4, 4, alignedRng);
        TEST_ASSERT_TRUE((alignedState.step()) == (4));
        TEST_ASSERT_TRUE((alignedState.iteration()) == (uint32_t(7)));
    
}

void test_10_advancealigned_pendulum_and_pingpong_honor_their_distinct_endpoint_formulas() {
        SequenceState pendulum;
        SequenceState pingPong;
        Random pendulumRng = seeded(0);
        Random pingPongRng = seeded(0);
        pendulum.reset();
        pingPong.reset();

        pendulum.advanceAligned(3, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (4));
        TEST_ASSERT_TRUE((pendulum.iteration()) == (uint32_t(0)));

        pendulum.advanceAligned(5, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (2));
        TEST_ASSERT_TRUE((pendulum.iteration()) == (uint32_t(0)));

        pendulum.advanceAligned(6, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        TEST_ASSERT_TRUE((pendulum.step()) == (2));
        TEST_ASSERT_TRUE((pendulum.iteration()) == (uint32_t(1)));

        pingPong.advanceAligned(3, Types::RunMode::PingPong, 2, 4, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (3));
        TEST_ASSERT_TRUE((pingPong.iteration()) == (uint32_t(0)));

        pingPong.advanceAligned(4, Types::RunMode::PingPong, 2, 4, pingPongRng);
        TEST_ASSERT_TRUE((pingPong.step()) == (2));
        TEST_ASSERT_TRUE((pingPong.iteration()) == (uint32_t(1)));
    
}

void test_11_advancealigned_random_randomwalk_and_last_cover_their_dedicated_branches() {
        SequenceState randomState;
        SequenceState walkState;
        SequenceState lastState;
        Random randomRng = seeded(0);
        Random expectedRandomRng = seeded(0);
        Random walkRng = seeded(0);
        Random expectedWalkRng = seeded(0);
        Random lastRng = seeded(0);
        randomState.reset();
        walkState.reset();
        lastState.reset();

        const int expectedRandomStep = int(expectedRandomRng.nextRange(3)) + 3;
        randomState.advanceAligned(99, Types::RunMode::Random, 3, 5, randomRng);
        TEST_ASSERT_TRUE((randomState.step()) == (expectedRandomStep));

        int expectedWalkStep = int(expectedWalkRng.nextRange(2));
        walkState.advanceAligned(0, Types::RunMode::RandomWalk, 0, 1, walkRng);
        TEST_ASSERT_TRUE((walkState.step()) == (expectedWalkStep));

        const int walkDirection = int(expectedWalkRng.nextRange(2));
        if (walkDirection == 0) {
            expectedWalkStep = expectedWalkStep == 0 ? 1 : expectedWalkStep - 1;
        } else {
            expectedWalkStep = expectedWalkStep == 1 ? 0 : expectedWalkStep + 1;
        }
        walkState.advanceAligned(1, Types::RunMode::RandomWalk, 0, 1, walkRng);
        TEST_ASSERT_TRUE((walkState.step()) == (expectedWalkStep));

        lastState.advanceAligned(7, Types::RunMode::Forward, 0, 3, lastRng);
        TEST_ASSERT_TRUE((lastState.step()) == (3));
        lastState.advanceAligned(123, Types::RunMode::Last, 0, 3, lastRng);
        TEST_ASSERT_TRUE((lastState.prevStep()) == (3));
        TEST_ASSERT_TRUE((lastState.step()) == (3));
    
}

void test_12_advancefree_random_also_reselects_a_step_on_non_initial_calls() {
        SequenceState state;
        Random rng = seeded(0);
        Random expected = seeded(0);
        state.reset();

        const int firstExpected = int(expected.nextRange(3)) + 3;
        state.advanceFree(Types::RunMode::Random, 3, 5, rng);
        TEST_ASSERT_TRUE((state.step()) == (firstExpected));
        TEST_ASSERT_TRUE((state.prevStep()) == (-1));

        const int secondExpected = int(expected.nextRange(3)) + 3;
        state.advanceFree(Types::RunMode::Random, 3, 5, rng);
        TEST_ASSERT_TRUE((state.prevStep()) == (firstExpected));
        TEST_ASSERT_TRUE((state.step()) == (secondExpected));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_reset_initializes_step_history_direction_and_iteration);
    RUN_TEST(test_02_advancefree_forward_wraps_to_first_step_and_increments_iteration);
    RUN_TEST(test_03_advancefree_backward_wraps_to_last_step_and_increments_iteration);
    RUN_TEST(test_04_advancefree_pendulum_holds_endpoints_while_pingpong_bounces_immediately);
    RUN_TEST(test_05_advancefree_clamps_out_of_range_current_step_before_advancing);
    RUN_TEST(test_06_advancefree_random_follows_random_and_randomwalk_wraps_at_both_boundaries);
    RUN_TEST(test_07_advancefree_run_mode_last_preserves_the_current_step);
    RUN_TEST(test_08_advancealigned_forward_and_backward_map_absolute_step_and_iteration_deterministically);
    RUN_TEST(test_09_single_step_pingpong_remains_stable_in_free_and_aligned_modes);
    RUN_TEST(test_10_advancealigned_pendulum_and_pingpong_honor_their_distinct_endpoint_formulas);
    RUN_TEST(test_11_advancealigned_random_randomwalk_and_last_cover_their_dedicated_branches);
    RUN_TEST(test_12_advancefree_random_also_reselects_a_step_on_non_initial_calls);
    return UNITY_END();
}

