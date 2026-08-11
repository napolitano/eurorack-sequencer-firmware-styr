/**
 * @file TestAdc.cpp
 * @brief Implements the TestAdc component used by the integration test.
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

#include "drivers/Adc.h"

class TestAdc : public IntegrationTest {
public:
    void init() override {
        adc.init();
    }

    void update() override {
        for (int c = 0; c < Adc::Channels; ++c) {
            dbg_printf("adc%d: %d ", c, adc.channel(c));
        }
        dbg_printf("\n");
    }

private:
    Adc adc;
};

INTEGRATION_TEST(TestAdc, "Adc", true)
