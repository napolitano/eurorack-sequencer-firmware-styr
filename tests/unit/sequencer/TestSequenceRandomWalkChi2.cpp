/**
 * @file TestSequenceRandomWalkChi2.cpp
 * @brief Implements the TestSequenceRandomWalkChi2 component used by the unit test.
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
#include "model/Types.h"
#include "core/utils/Random.h"

#include <cmath>

UNIT_TEST("SequenceRandomWalkChi2") {

    CASE("Per-state RandomWalk next-step distribution matches 50/50 via z-test") {
        const int first = 0;
        const int last = 7; // 8 steps
        const int stepCount = last - first + 1;

        const int N = 5000; // trials per state
        const double alpha = 0.01; // significance level

        for (int s = first; s <= last; ++s) {
            int incCount = 0;
            int decCount = 0;
            for (int t = 0; t < N; ++t) {
                // independent RNG per trial to ensure independent samples
                Random rng(uint32_t(0x1000u + uint32_t(s) * 131u + uint32_t(t)));

                SequenceState st;
                // set deterministic current step to s
                st.reset();
                st.advanceAligned(s - first, Types::RunMode::Forward, first, last, rng);

                // draw next via RandomWalk
                st.advanceFree(Types::RunMode::RandomWalk, first, last, rng);
                int cur = st.step();

                int expectedInc = (s == last) ? first : s + 1;
                int expectedDec = (s == first) ? last : s - 1;
                if (cur == expectedInc) ++incCount;
                else if (cur == expectedDec) ++decCount;
                else {
                    // should never happen
                    expectFalse(true);
                }
            }

            // Binomial test approximation via z-score
            double mean = double(N) * 0.5;
            double var = double(N) * 0.25; // N*p*(1-p)
            double z = (double(incCount) - mean) / std::sqrt(var);
            double absz = std::abs(z);
            // two-sided p-value from normal approx using erf
            double pval = 2.0 * (1.0 - 0.5 * (1.0 + std::erf(absz / std::sqrt(2.0))));

            // Expect we cannot reject null (p > alpha)
            expectTrue(pval > alpha);
        }
    }

}

