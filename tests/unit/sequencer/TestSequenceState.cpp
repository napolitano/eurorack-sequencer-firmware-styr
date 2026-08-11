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
#include "UnitTest.h"

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

UNIT_TEST("SequenceState") {

    CASE("reset initializes step history direction and iteration") {
        SequenceState state;
        state.reset();

        expectEqual(state.step(), -1);
        expectEqual(state.prevStep(), -1);
        expectEqual(state.direction(), 1);
        expectEqual(state.iteration(), uint32_t(0));
    }

    CASE("advanceFree forward wraps to first step and increments iteration") {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        expectEqual(state.step(), 2);
        expectEqual(state.prevStep(), -1);
        expectEqual(state.iteration(), uint32_t(0));

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        expectEqual(state.step(), 3);
        expectEqual(state.prevStep(), 2);

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        expectEqual(state.step(), 4);
        expectEqual(state.prevStep(), 3);

        state.advanceFree(Types::RunMode::Forward, 2, 4, rng);
        expectEqual(state.step(), 2);
        expectEqual(state.prevStep(), 4);
        expectEqual(state.iteration(), uint32_t(1));
    }

    CASE("advanceFree backward wraps to last step and increments iteration") {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        expectEqual(state.step(), 4);
        expectEqual(state.prevStep(), -1);

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        expectEqual(state.step(), 3);
        expectEqual(state.prevStep(), 4);

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        expectEqual(state.step(), 2);
        expectEqual(state.prevStep(), 3);

        state.advanceFree(Types::RunMode::Backward, 2, 4, rng);
        expectEqual(state.step(), 4);
        expectEqual(state.prevStep(), 2);
        expectEqual(state.iteration(), uint32_t(1));
    }

    CASE("advanceFree pendulum holds endpoints while pingpong bounces immediately") {
        SequenceState pendulum;
        SequenceState pingPong;
        Random pendulumRng = seeded(0);
        Random pingPongRng = seeded(0);
        pendulum.reset();
        pingPong.reset();

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        expectEqual(pendulum.step(), 2);
        expectEqual(pendulum.direction(), 1);

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        expectEqual(pendulum.step(), 2);
        expectEqual(pendulum.direction(), -1);

        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        expectEqual(pendulum.step(), 1);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        expectEqual(pendulum.step(), 0);
        pendulum.advanceFree(Types::RunMode::Pendulum, 0, 2, pendulumRng);
        expectEqual(pendulum.step(), 0);
        expectEqual(pendulum.direction(), 1);
        expectEqual(pendulum.iteration(), uint32_t(1));

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        expectEqual(pingPong.step(), 2);
        expectEqual(pingPong.direction(), 1);

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        expectEqual(pingPong.step(), 1);
        expectEqual(pingPong.direction(), -1);

        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        expectEqual(pingPong.step(), 0);
        pingPong.advanceFree(Types::RunMode::PingPong, 0, 2, pingPongRng);
        expectEqual(pingPong.step(), 1);
        expectEqual(pingPong.direction(), 1);
        expectEqual(pingPong.iteration(), uint32_t(1));
    }

    CASE("advanceFree clamps out-of-range current step before advancing") {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Backward, 0, 5, rng);
        expectEqual(state.step(), 5);

        state.advanceFree(Types::RunMode::Forward, 2, 3, rng);
        expectEqual(state.prevStep(), 5);
        expectEqual(state.step(), 2);
        expectEqual(state.iteration(), uint32_t(1));
    }

    CASE("advanceFree random follows Random and randomWalk wraps at both boundaries") {
        SequenceState randomState;
        Random randomRng = seeded(0);
        Random expectedRandomRng = seeded(0);
        randomState.reset();

        const int expectedRandomStep = int(expectedRandomRng.nextRange(3)) + 3;
        randomState.advanceFree(Types::RunMode::Random, 3, 5, randomRng);
        expectEqual(randomState.step(), expectedRandomStep);
        expectEqual(randomState.prevStep(), -1);

        // Put the state on the left boundary without consuming the RNG, then
        // choose a seed whose first binary draw selects the negative direction.
        const uint32_t leftSeed = seedSelecting(2, 0);
        expect(leftSeed != uint32_t(-1));
        SequenceState walkLeftWrap;
        Random leftWrapRng = seeded(leftSeed);
        walkLeftWrap.reset();
        walkLeftWrap.advanceFree(Types::RunMode::Forward, 0, 1, leftWrapRng);
        expectEqual(walkLeftWrap.step(), 0);
        walkLeftWrap.advanceFree(Types::RunMode::RandomWalk, 0, 1, leftWrapRng);
        expectEqual(walkLeftWrap.step(), 1);

        // Likewise exercise the right-boundary wrap explicitly with a seed
        // whose first binary draw selects the positive direction.
        const uint32_t rightSeed = seedSelecting(2, 1);
        expect(rightSeed != uint32_t(-1));
        SequenceState walkRightWrap;
        Random rightWrapRng = seeded(rightSeed);
        walkRightWrap.reset();
        walkRightWrap.advanceFree(Types::RunMode::Backward, 0, 1, rightWrapRng);
        expectEqual(walkRightWrap.step(), 1);
        walkRightWrap.advanceFree(Types::RunMode::RandomWalk, 0, 1, rightWrapRng);
        expectEqual(walkRightWrap.step(), 0);
    }

    CASE("advanceFree run mode Last preserves the current step") {
        SequenceState state;
        Random rng = seeded(0);
        state.reset();

        state.advanceFree(Types::RunMode::Last, 0, 3, rng);
        expectEqual(state.step(), -1);
        expectEqual(state.prevStep(), -1);

        state.advanceFree(Types::RunMode::Forward, 0, 3, rng);
        expectEqual(state.step(), 0);
        state.advanceFree(Types::RunMode::Last, 0, 3, rng);
        expectEqual(state.prevStep(), 0);
        expectEqual(state.step(), 0);
    }

    CASE("advanceAligned forward and backward map absolute step and iteration deterministically") {
        SequenceState forward;
        SequenceState backward;
        Random rng0 = seeded(0);
        Random rng1 = seeded(0);
        forward.reset();
        backward.reset();

        forward.advanceAligned(5, Types::RunMode::Forward, 2, 4, rng0);
        expectEqual(forward.step(), 4);
        expectEqual(forward.prevStep(), -1);
        expectEqual(forward.iteration(), uint32_t(1));

        backward.advanceAligned(4, Types::RunMode::Backward, 2, 4, rng1);
        expectEqual(backward.step(), 3);
        expectEqual(backward.prevStep(), -1);
        expectEqual(backward.iteration(), uint32_t(1));
    }

    CASE("single-step pingpong remains stable in free and aligned modes") {
        SequenceState freeState;
        SequenceState alignedState;
        Random freeRng = seeded(0);
        Random alignedRng = seeded(0);
        freeState.reset();
        alignedState.reset();

        freeState.advanceFree(Types::RunMode::PingPong, 4, 4, freeRng);
        expectEqual(freeState.step(), 4);
        expectEqual(freeState.iteration(), uint32_t(0));

        freeState.advanceFree(Types::RunMode::PingPong, 4, 4, freeRng);
        expectEqual(freeState.step(), 4);
        expectEqual(freeState.iteration(), uint32_t(1));

        alignedState.advanceAligned(0, Types::RunMode::PingPong, 4, 4, alignedRng);
        expectEqual(alignedState.step(), 4);
        expectEqual(alignedState.iteration(), uint32_t(0));

        alignedState.advanceAligned(7, Types::RunMode::PingPong, 4, 4, alignedRng);
        expectEqual(alignedState.step(), 4);
        expectEqual(alignedState.iteration(), uint32_t(7));
    }

    CASE("advanceAligned pendulum and pingpong honor their distinct endpoint formulas") {
        SequenceState pendulum;
        SequenceState pingPong;
        Random pendulumRng = seeded(0);
        Random pingPongRng = seeded(0);
        pendulum.reset();
        pingPong.reset();

        pendulum.advanceAligned(3, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        expectEqual(pendulum.step(), 4);
        expectEqual(pendulum.iteration(), uint32_t(0));

        pendulum.advanceAligned(5, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        expectEqual(pendulum.step(), 2);
        expectEqual(pendulum.iteration(), uint32_t(0));

        pendulum.advanceAligned(6, Types::RunMode::Pendulum, 2, 4, pendulumRng);
        expectEqual(pendulum.step(), 2);
        expectEqual(pendulum.iteration(), uint32_t(1));

        pingPong.advanceAligned(3, Types::RunMode::PingPong, 2, 4, pingPongRng);
        expectEqual(pingPong.step(), 3);
        expectEqual(pingPong.iteration(), uint32_t(0));

        pingPong.advanceAligned(4, Types::RunMode::PingPong, 2, 4, pingPongRng);
        expectEqual(pingPong.step(), 2);
        expectEqual(pingPong.iteration(), uint32_t(1));
    }

    CASE("advanceAligned random randomWalk and Last cover their dedicated branches") {
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
        expectEqual(randomState.step(), expectedRandomStep);

        int expectedWalkStep = int(expectedWalkRng.nextRange(2));
        walkState.advanceAligned(0, Types::RunMode::RandomWalk, 0, 1, walkRng);
        expectEqual(walkState.step(), expectedWalkStep);

        const int walkDirection = int(expectedWalkRng.nextRange(2));
        if (walkDirection == 0) {
            expectedWalkStep = expectedWalkStep == 0 ? 1 : expectedWalkStep - 1;
        } else {
            expectedWalkStep = expectedWalkStep == 1 ? 0 : expectedWalkStep + 1;
        }
        walkState.advanceAligned(1, Types::RunMode::RandomWalk, 0, 1, walkRng);
        expectEqual(walkState.step(), expectedWalkStep);

        lastState.advanceAligned(7, Types::RunMode::Forward, 0, 3, lastRng);
        expectEqual(lastState.step(), 3);
        lastState.advanceAligned(123, Types::RunMode::Last, 0, 3, lastRng);
        expectEqual(lastState.prevStep(), 3);
        expectEqual(lastState.step(), 3);
    }

    CASE("advanceFree random also reselects a step on non-initial calls") {
        SequenceState state;
        Random rng = seeded(0);
        Random expected = seeded(0);
        state.reset();

        const int firstExpected = int(expected.nextRange(3)) + 3;
        state.advanceFree(Types::RunMode::Random, 3, 5, rng);
        expectEqual(state.step(), firstExpected);
        expectEqual(state.prevStep(), -1);

        const int secondExpected = int(expected.nextRange(3)) + 3;
        state.advanceFree(Types::RunMode::Random, 3, 5, rng);
        expectEqual(state.prevStep(), firstExpected);
        expectEqual(state.step(), secondExpected);
    }
}
