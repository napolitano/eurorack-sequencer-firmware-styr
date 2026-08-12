/**
 * @file TestCvInput.cpp
 * @brief Implements the TestCvInput component used by the unit test.
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

#include "engine/CvInput.h"

#include "core/Simulator.h"

#include <cmath>
#include <memory>

namespace {

class CvInputHarness {
public:
    CvInputHarness() :
        _simulator(makeTarget())
    {
        _simulator.reboot();
    }

    CvInput &cvInput() { return *_cvInput; }

    void setVoltage(int channel, float voltage) {
        _simulator.setAdc(channel, voltage);
    }

private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = [this] () {
            _adc.reset(new Adc());
            _cvInput.reset(new CvInput(*_adc));
        };
        target.destroy = [this] () {
            _cvInput.reset();
            _adc.reset();
        };
        target.update = [] () {};
        return target;
    }

    std::unique_ptr<Adc> _adc;
    std::unique_ptr<CvInput> _cvInput;
    sim::Simulator _simulator;
};

static float expectedChannel(float voltage) {
    float normalized = std::max(0.f, std::min(1.f, voltage * 0.1f + 0.5f));
    uint16_t raw = uint16_t(std::floor(0xffff - 0xffff * normalized));
    return 5.f - raw / 6553.5f;
}

static void expectClose(float actual, float expected, float epsilon = 0.0002f) {
    TEST_ASSERT_TRUE(std::fabs(actual - expected) <= epsilon);
}

} // namespace

void test_01_init_zeros_all_exposed_channels() {
        CvInputHarness harness;
        auto &cvInput = harness.cvInput();

        cvInput.init();

        for (int channel = 0; channel < CvInput::Channels; ++channel) {
            TEST_ASSERT_TRUE((cvInput.channel(channel)) == (0.f));
        }
    
}

void test_02_update_converts_adc_voltages_for_each_channel() {
        CvInputHarness harness;
        auto &cvInput = harness.cvInput();

        harness.setVoltage(0, -5.f);
        harness.setVoltage(1, -1.25f);
        harness.setVoltage(2, 0.f);
        harness.setVoltage(3, 2.5f);

        cvInput.update();

        expectClose(cvInput.channel(0), expectedChannel(-5.f));
        expectClose(cvInput.channel(1), expectedChannel(-1.25f));
        expectClose(cvInput.channel(2), expectedChannel(0.f));
        expectClose(cvInput.channel(3), expectedChannel(2.5f));
    
}

void test_03_update_clamps_voltages_outside_the_supported_range() {
        CvInputHarness harness;
        auto &cvInput = harness.cvInput();

        harness.setVoltage(0, -12.f);
        harness.setVoltage(1, 12.f);
        cvInput.update();

        expectClose(cvInput.channel(0), expectedChannel(-5.f));
        expectClose(cvInput.channel(1), expectedChannel(5.f));

        cvInput.init();
        TEST_ASSERT_TRUE((cvInput.channel(0)) == (0.f));
        TEST_ASSERT_TRUE((cvInput.channel(1)) == (0.f));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_init_zeros_all_exposed_channels);
    RUN_TEST(test_02_update_converts_adc_voltages_for_each_channel);
    RUN_TEST(test_03_update_clamps_voltages_outside_the_supported_range);
    return UNITY_END();
}


