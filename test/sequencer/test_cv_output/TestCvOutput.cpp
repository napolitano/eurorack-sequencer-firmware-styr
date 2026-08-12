/**
 * @file TestCvOutput.cpp
 * @brief Implements the TestCvOutput component used by the unit test.
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

#include "engine/CvOutput.h"

#include "core/Simulator.h"

#include <memory>

namespace {

class CvOutputHarness {
public:
    CvOutputHarness() :
        _simulator(makeTarget())
    {
        _calibration.clear();
        _simulator.reboot();
    }

    CvOutput &cvOutput() { return *_cvOutput; }
    const Calibration &calibration() const { return _calibration; }
    const sim::TargetState &targetState() const { return _simulator.targetState(); }

private:
    sim::Target makeTarget() {
        sim::Target target;
        target.create = [this] () {
            _dac.reset(new Dac());
            _cvOutput.reset(new CvOutput(*_dac, _calibration));
        };
        target.destroy = [this] () {
            _cvOutput.reset();
            _dac.reset();
        };
        target.update = [] () {};
        return target;
    }

    Calibration _calibration;
    std::unique_ptr<Dac> _dac;
    std::unique_ptr<CvOutput> _cvOutput;
    sim::Simulator _simulator;
};

static uint16_t expectedValue(const Calibration &calibration, int channel, float volts) {
    return calibration.cvOutput(channel).voltsToValue(volts);
}

} // namespace

void test_01_init_zeros_all_logical_output_channels() {
        CvOutputHarness harness;
        auto &cvOutput = harness.cvOutput();

        cvOutput.init();

        for (int channel = 0; channel < CvOutput::Channels; ++channel) {
            TEST_ASSERT_TRUE((cvOutput.channel(channel)) == (0.f));
        }
    
}

void test_02_update_writes_calibrated_dac_values_for_all_zeroed_channels() {
        CvOutputHarness harness;
        auto &cvOutput = harness.cvOutput();

        cvOutput.init();
        cvOutput.update();

        for (int channel = 0; channel < CvOutput::Channels; ++channel) {
            TEST_ASSERT_TRUE((harness.targetState().dac.state[channel]) == (expectedValue(harness.calibration(), channel, 0.f)));
        }
    
}

void test_03_update_converts_representative_voltages_and_clamps_out_of_range_values() {
        CvOutputHarness harness;
        auto &cvOutput = harness.cvOutput();

        cvOutput.init();
        cvOutput.setChannel(0, -5.f);
        cvOutput.setChannel(1, -1.5f);
        cvOutput.setChannel(2, 2.25f);
        cvOutput.setChannel(3, 9.f);
        cvOutput.update();

        TEST_ASSERT_TRUE((cvOutput.channel(0)) == (-5.f));
        TEST_ASSERT_TRUE((cvOutput.channel(1)) == (-1.5f));
        TEST_ASSERT_TRUE((cvOutput.channel(2)) == (2.25f));
        TEST_ASSERT_TRUE((cvOutput.channel(3)) == (9.f));

        TEST_ASSERT_TRUE((harness.targetState().dac.state[0]) == (expectedValue(harness.calibration(), 0, -5.f)));
        TEST_ASSERT_TRUE((harness.targetState().dac.state[1]) == (expectedValue(harness.calibration(), 1, -1.5f)));
        TEST_ASSERT_TRUE((harness.targetState().dac.state[2]) == (expectedValue(harness.calibration(), 2, 2.25f)));
        TEST_ASSERT_TRUE((harness.targetState().dac.state[3]) == (expectedValue(harness.calibration(), 3, 5.f)));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_init_zeros_all_logical_output_channels);
    RUN_TEST(test_02_update_writes_calibrated_dac_values_for_all_zeroed_channels);
    RUN_TEST(test_03_update_converts_representative_voltages_and_clamps_out_of_range_values);
    return UNITY_END();
}


