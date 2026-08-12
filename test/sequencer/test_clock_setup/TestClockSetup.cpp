/**
 * @file TestClockSetup.cpp
 * @brief Implements the TestClockSetup component used by the unit test.
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

#include "MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"
#include "core/utils/StringBuilder.h"

#include "model/ClockSetup.h"
#include "model/ProjectVersion.h"
#include "model/ModelUtils.h"

#include <cstring>

static uint8_t buf[128];

static void clearBuf() {
    std::memset(buf, 0, sizeof(buf));
}

static void writeClockSetupV10() {
    MemoryWriter memoryWriter(buf, sizeof(buf));
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 10);

    writer.write(ClockSetup::Mode::Slave);
    writer.write(ClockSetup::ShiftMode::Pause);
    writer.write(uint8_t(24));
    writer.write(ClockSetup::ClockInputMode::Run);
    writer.write(uint8_t(48));
    writer.write(uint8_t(7));
    writer.write(ClockSetup::ClockOutputMode::Run);
    writer.write(true);
    writer.write(false);
    writer.write(true);
    writer.write(false);
}

static void writeClockSetupV11() {
    MemoryWriter memoryWriter(buf, sizeof(buf));
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 11);

    writer.write(ClockSetup::Mode::Master);
    writer.write(ClockSetup::ShiftMode::Restart);
    writer.write(uint8_t(12));
    writer.write(ClockSetup::ClockInputMode::StartStop);
    writer.write(uint8_t(24));
    writer.write(true);
    writer.write(uint8_t(5));
    writer.write(ClockSetup::ClockOutputMode::Reset);
    writer.write(false);
    writer.write(true);
    writer.write(false);
    writer.write(true);
}

static void writeClockSetupV11NoSwing() {
    MemoryWriter memoryWriter(buf, sizeof(buf));
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 11);

    writer.write(ClockSetup::Mode::Master);
    writer.write(ClockSetup::ShiftMode::Restart);
    writer.write(uint8_t(12));
    writer.write(ClockSetup::ClockInputMode::StartStop);
    writer.write(uint8_t(24));
    writer.write(false);
    writer.write(uint8_t(5));
    writer.write(ClockSetup::ClockOutputMode::Reset);
    writer.write(false);
    writer.write(true);
    writer.write(false);
    writer.write(true);
}

void test_01_clear_initializes_documented_defaults() {
        ClockSetup setup;
        setup.clear();

        TEST_ASSERT_TRUE((setup.mode()) == (ClockSetup::Mode::Auto));
        TEST_ASSERT_TRUE((setup.shiftMode()) == (ClockSetup::ShiftMode::Restart));
        TEST_ASSERT_TRUE((setup.clockInputDivisor()) == (12));
        TEST_ASSERT_TRUE((setup.clockInputMode()) == (ClockSetup::ClockInputMode::Reset));
        TEST_ASSERT_TRUE((setup.clockOutputDivisor()) == (12));
        TEST_ASSERT_FALSE(setup.clockOutputSwing());
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (1));
        TEST_ASSERT_TRUE((setup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Reset));
        TEST_ASSERT_TRUE(setup.midiRx());
        TEST_ASSERT_TRUE(setup.midiTx());
        TEST_ASSERT_FALSE(setup.usbRx());
        TEST_ASSERT_FALSE(setup.usbTx());
        TEST_ASSERT_TRUE(setup.isDirty());
    
}

void test_02_divisors_clamp_to_current_valid_ranges() {
        ClockSetup setup;
        setup.clear();

        setup.setClockInputDivisor(-99);
        TEST_ASSERT_TRUE((setup.clockInputDivisor()) == (1));

        setup.setClockInputDivisor(999);
        TEST_ASSERT_TRUE((setup.clockInputDivisor()) == (192));

        setup.setClockOutputDivisor(-99);
        TEST_ASSERT_TRUE((setup.clockOutputDivisor()) == (2));

        setup.setClockOutputDivisor(999);
        TEST_ASSERT_TRUE((setup.clockOutputDivisor()) == (192));
    
}

void test_03_input_divisor_string_formatting_stays_stable() {
        ClockSetup setup;
        setup.clear();

        FixedStringBuilder<32> str;
        setup.printClockInputDivisor(str);
        TEST_ASSERT_EQUAL_STRING("12 1/16", (const char *)str);

        str.reset();
        setup.setClockInputDivisor(16);
        setup.printClockInputDivisor(str);
        TEST_ASSERT_EQUAL_STRING("16 1/8T", (const char *)str);

        str.reset();
        setup.setClockInputDivisor(17);
        setup.printClockInputDivisor(str);
        TEST_ASSERT_EQUAL_STRING("17", (const char *)str);
    
}

void test_04_reading_v10_data_keeps_clockoutputswing_default() {
        clearBuf();
        writeClockSetupV10();

        ClockSetup setup;
        setup.clear();

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Latest);
        setup.read(reader);

        TEST_ASSERT_TRUE((setup.mode()) == (ClockSetup::Mode::Slave));
        TEST_ASSERT_TRUE((setup.shiftMode()) == (ClockSetup::ShiftMode::Pause));
        TEST_ASSERT_TRUE((setup.clockInputDivisor()) == (24));
        TEST_ASSERT_TRUE((setup.clockInputMode()) == (ClockSetup::ClockInputMode::Run));
        TEST_ASSERT_TRUE((setup.clockOutputDivisor()) == (48));
        TEST_ASSERT_FALSE(setup.clockOutputSwing());
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (7));
        TEST_ASSERT_TRUE((setup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Run));
        TEST_ASSERT_TRUE(setup.midiRx());
        TEST_ASSERT_FALSE(setup.midiTx());
        TEST_ASSERT_TRUE(setup.usbRx());
        TEST_ASSERT_FALSE(setup.usbTx());
    
}

void test_05_reading_v11_data_restores_clockoutputswing() {
        clearBuf();
        writeClockSetupV11();

        ClockSetup setup;
        setup.clear();

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Latest);
        setup.read(reader);

        TEST_ASSERT_TRUE((setup.mode()) == (ClockSetup::Mode::Master));
        TEST_ASSERT_TRUE((setup.shiftMode()) == (ClockSetup::ShiftMode::Restart));
        TEST_ASSERT_TRUE((setup.clockInputDivisor()) == (12));
        TEST_ASSERT_TRUE((setup.clockInputMode()) == (ClockSetup::ClockInputMode::StartStop));
        TEST_ASSERT_TRUE((setup.clockOutputDivisor()) == (24));
        TEST_ASSERT_TRUE(setup.clockOutputSwing());
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (5));
        TEST_ASSERT_TRUE((setup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Reset));
        TEST_ASSERT_FALSE(setup.midiRx());
        TEST_ASSERT_TRUE(setup.midiTx());
        TEST_ASSERT_FALSE(setup.usbRx());
        TEST_ASSERT_TRUE(setup.usbTx());
    
}

void test_06_latest_write_read_roundtrip_preserves_all_clock_setup_fields() {
        clearBuf();

        ClockSetup writeSetup;
        writeSetup.clear();
        writeSetup.setMode(ClockSetup::Mode::Slave);
        writeSetup.setShiftMode(ClockSetup::ShiftMode::Pause);
        writeSetup.setClockInputDivisor(23);
        writeSetup.setClockInputMode(ClockSetup::ClockInputMode::StartStop);
        writeSetup.setClockOutputDivisor(47);
        writeSetup.setClockOutputSwing(true);
        writeSetup.setClockOutputPulse(9);
        writeSetup.setClockOutputMode(ClockSetup::ClockOutputMode::Run);
        writeSetup.setMidiRx(false);
        writeSetup.setMidiTx(true);
        writeSetup.setUsbRx(true);
        writeSetup.setUsbTx(false);

        MemoryWriter memoryWriter(buf, sizeof(buf));
        VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, ProjectVersion::Latest);
        writeSetup.write(writer);

        ClockSetup readSetup;
        readSetup.clear();
        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Latest);
        readSetup.read(reader);

        TEST_ASSERT_TRUE((readSetup.mode()) == (ClockSetup::Mode::Slave));
        TEST_ASSERT_TRUE((readSetup.shiftMode()) == (ClockSetup::ShiftMode::Pause));
        TEST_ASSERT_TRUE((readSetup.clockInputDivisor()) == (23));
        TEST_ASSERT_TRUE((readSetup.clockInputMode()) == (ClockSetup::ClockInputMode::StartStop));
        TEST_ASSERT_TRUE((readSetup.clockOutputDivisor()) == (47));
        TEST_ASSERT_TRUE(readSetup.clockOutputSwing());
        TEST_ASSERT_TRUE((readSetup.clockOutputPulse()) == (9));
        TEST_ASSERT_TRUE((readSetup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Run));
        TEST_ASSERT_FALSE(readSetup.midiRx());
        TEST_ASSERT_TRUE(readSetup.midiTx());
        TEST_ASSERT_TRUE(readSetup.usbRx());
        TEST_ASSERT_FALSE(readSetup.usbTx());

        TEST_ASSERT_TRUE((memoryWriter.bytesWritten()) == (memoryReader.bytesRead()));
    
}

void test_07_read_does_not_implicitly_change_dirty_flag_state() {
        clearBuf();
        writeClockSetupV11();

        ClockSetup setup;
        setup.clear();
        setup.clearDirty();
        TEST_ASSERT_FALSE(setup.isDirty());

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, ProjectVersion::Latest);
        setup.read(reader);

        TEST_ASSERT_FALSE(setup.isDirty());
    
}

void test_08_version_boundary_v10_ignores_swing_bit_v11_reads_swing_bit() {
        ClockSetup v10Setup;
        v10Setup.clear();
        clearBuf();
        writeClockSetupV10();
        MemoryReader v10ReaderMem(buf, sizeof(buf));
        VersionedSerializedReader v10Reader([&v10ReaderMem] (void *data, size_t len) { v10ReaderMem.read(data, len); }, ProjectVersion::Latest);
        v10Setup.read(v10Reader);
        TEST_ASSERT_FALSE(v10Setup.clockOutputSwing());

        ClockSetup v11Setup;
        v11Setup.clear();
        clearBuf();
        writeClockSetupV11NoSwing();
        MemoryReader v11ReaderMem(buf, sizeof(buf));
        VersionedSerializedReader v11Reader([&v11ReaderMem] (void *data, size_t len) { v11ReaderMem.read(data, len); }, ProjectVersion::Latest);
        v11Setup.read(v11Reader);
        TEST_ASSERT_FALSE(v11Setup.clockOutputSwing());

        clearBuf();
        writeClockSetupV11();
        MemoryReader v11ReaderMemSwing(buf, sizeof(buf));
        VersionedSerializedReader v11ReaderSwing([&v11ReaderMemSwing] (void *data, size_t len) { v11ReaderMemSwing.read(data, len); }, ProjectVersion::Latest);
        v11Setup.read(v11ReaderSwing);
        TEST_ASSERT_TRUE(v11Setup.clockOutputSwing());
    
}

void test_09_dirty_flag_only_flips_on_actual_value_changes() {
        ClockSetup setup;
        setup.clear();
        TEST_ASSERT_TRUE(setup.isDirty());

        setup.clearDirty();
        TEST_ASSERT_FALSE(setup.isDirty());

        setup.setClockInputDivisor(setup.clockInputDivisor());
        TEST_ASSERT_FALSE(setup.isDirty());

        setup.setClockInputDivisor(setup.clockInputDivisor() + 1);
        TEST_ASSERT_TRUE(setup.isDirty());

        setup.clearDirty();
        setup.setClockOutputSwing(setup.clockOutputSwing());
        TEST_ASSERT_FALSE(setup.isDirty());

        setup.setClockOutputSwing(!setup.clockOutputSwing());
        TEST_ASSERT_TRUE(setup.isDirty());

        setup.clearDirty();
        setup.setMode(setup.mode());
        TEST_ASSERT_FALSE(setup.isDirty());

        setup.setMode(ClockSetup::Mode::Slave);
        TEST_ASSERT_TRUE(setup.isDirty());

        setup.clearDirty();
        setup.setMidiRx(setup.midiRx());
        TEST_ASSERT_FALSE(setup.isDirty());

        setup.setMidiRx(!setup.midiRx());
        TEST_ASSERT_TRUE(setup.isDirty());
    
}

void test_10_clock_output_pulse_clamps_to_1_20() {
        ClockSetup setup;
        setup.clear();

        setup.setClockOutputPulse(7);
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (7));

        setup.setClockOutputPulse(-99);
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (1));

        setup.setClockOutputPulse(999);
        TEST_ASSERT_TRUE((setup.clockOutputPulse()) == (20));
    
}

void test_11_output_divisor_and_pulse_string_formatting_stays_stable() {
        ClockSetup setup;
        setup.clear();

        FixedStringBuilder<32> outDiv;
        setup.setClockOutputDivisor(24);
        setup.printClockOutputDivisor(outDiv);
        TEST_ASSERT_EQUAL_STRING("24 1/8", (const char *)outDiv);

        FixedStringBuilder<32> pulse;
        setup.setClockOutputPulse(9);
        setup.printClockOutputPulse(pulse);
        TEST_ASSERT_EQUAL_STRING("9ms", (const char *)pulse);
    
}

void test_12_enum_setters_clamp_wrapped_out_of_range_values_into_valid_enum_domain() {
        ClockSetup setup;
        setup.clear();

        setup.setMode(ClockSetup::Mode(-123));
        TEST_ASSERT_TRUE((setup.mode()) == (ClockSetup::Mode::Slave));
        setup.setMode(ClockSetup::Mode(123));
        TEST_ASSERT_TRUE((setup.mode()) == (ClockSetup::Mode::Slave));

        setup.setClockInputMode(ClockSetup::ClockInputMode(-123));
        TEST_ASSERT_TRUE((setup.clockInputMode()) == (ClockSetup::ClockInputMode::StartStop));
        setup.setClockInputMode(ClockSetup::ClockInputMode(123));
        TEST_ASSERT_TRUE((setup.clockInputMode()) == (ClockSetup::ClockInputMode::StartStop));

        setup.setClockOutputMode(ClockSetup::ClockOutputMode(-123));
        TEST_ASSERT_TRUE((setup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Run));
        setup.setClockOutputMode(ClockSetup::ClockOutputMode(123));
        TEST_ASSERT_TRUE((setup.clockOutputMode()) == (ClockSetup::ClockOutputMode::Run));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_clear_initializes_documented_defaults);
    RUN_TEST(test_02_divisors_clamp_to_current_valid_ranges);
    RUN_TEST(test_03_input_divisor_string_formatting_stays_stable);
    RUN_TEST(test_04_reading_v10_data_keeps_clockoutputswing_default);
    RUN_TEST(test_05_reading_v11_data_restores_clockoutputswing);
    RUN_TEST(test_06_latest_write_read_roundtrip_preserves_all_clock_setup_fields);
    RUN_TEST(test_07_read_does_not_implicitly_change_dirty_flag_state);
    RUN_TEST(test_08_version_boundary_v10_ignores_swing_bit_v11_reads_swing_bit);
    RUN_TEST(test_09_dirty_flag_only_flips_on_actual_value_changes);
    RUN_TEST(test_10_clock_output_pulse_clamps_to_1_20);
    RUN_TEST(test_11_output_divisor_and_pulse_string_formatting_stays_stable);
    RUN_TEST(test_12_enum_setters_clamp_wrapped_out_of_range_values_into_valid_enum_domain);
    return UNITY_END();
}


