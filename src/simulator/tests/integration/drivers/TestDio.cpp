/**
 * @file TestDio.cpp
 * @brief Implements the TestDio component used by the integration test.
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
#include "IntegrationTest.h"

#include "drivers/Dio.h"

class TestDio : public IntegrationTest {
public:
    void init() override {
        dio.init();

        dio.clockInput.setHandler([this] (bool) {
            ++clockInputCount;
        });
        dio.resetInput.setHandler([this] (bool) {
            ++resetInputCount;
        });
    }

    void update() override {
        DBG("clock input: %s (called: %d)", dio.clockInput.get() ? "high" : "low", clockInputCount);
        DBG("reset input: %s (called: %d)", dio.resetInput.get() ? "high" : "low", resetInputCount);
        DBG("clock output: %s", clockOutput ? "high" : "low");
        DBG("reset output: %s", clockOutput ? "high" : "low");
        dio.clockOutput.set(clockOutput);
        dio.resetOutput.set(resetOutput);
        clockOutput = !clockOutput;
        resetOutput = !resetOutput;
        os::delay(os::time::ms(500));
    }

private:
    Dio dio;
    bool clockOutput = false;
    bool resetOutput = true;
    int clockInputCount = 0;
    int resetInputCount = 0;
};

INTEGRATION_TEST(TestDio, "Dio", true)
