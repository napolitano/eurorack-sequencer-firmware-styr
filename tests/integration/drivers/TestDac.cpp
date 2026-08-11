/**
 * @file TestDac.cpp
 * @brief Implements the TestDac component used by the integration test.
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

#include "drivers/Dac.h"

class TestDac : public IntegrationTest {
public:
    void init() override {
        dac.init();
    }

    void update() override {
        DBG("value = %d", value);
        for (int channel = 0; channel < 8; ++channel) {
            dac.setValue(channel, (value + channel * 0x1000) % 0x8000);
        }
        dac.write();
        value = (value + 0x1000) % 0x8000;
        os::delay(os::time::ms(50));
    }

private:
    Dac dac;
    uint16_t value = 0;
};

INTEGRATION_TEST(TestDac, "Dac", true)
