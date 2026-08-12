/**
 * @file TestVersionedSerialization.cpp
 * @brief Implements the TestVersionedSerialization component used by the unit test.
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

#define VERSION(_x_) (_x_)

struct Data1 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    uint32_t field3 = 345;
};

struct Data2 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    int8_t field4 = -123; // new
    uint32_t field3 = 345;
};

struct Data3 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    int8_t field4 = -123;
    int16_t field5 = -234; // new
    uint32_t field3 = 345;
};

struct Data4 {
    uint8_t field1 = 123;
    uint16_t field2 = 234;
    // int8_t field4 = -123; // removed
    int16_t field5 = -234;
    uint32_t field3 = 345;
};

static void writeVersion1(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 1);
    Data1 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion2(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 2);
    Data2 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field4);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion3(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 3);
    Data3 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field4);
    writer.write(data.field5);
    writer.write(data.field3);
    writer.writeHash();
}

static void writeVersion4(void *buf, size_t len) {
    MemoryWriter memoryWriter(buf, len);
    VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) { memoryWriter.write(data, len); }, 4);
    Data4 data;
    writer.write(data.field1);
    writer.write(data.field2);
    writer.write(data.field5);
    writer.write(data.field3);
    writer.writeHash();
}

static void readVersion1(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 1);
    Data1 data {};
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field3);
    TEST_ASSERT_TRUE(reader.checkHash());
    Data1 expected;
    TEST_ASSERT_TRUE((data.field1) == (expected.field1));
    TEST_ASSERT_TRUE((data.field2) == (expected.field2));
    TEST_ASSERT_TRUE((data.field3) == (expected.field3));
}

static void readVersion2(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 2);
    Data2 data {};
    data.field4 = 0;
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field4, VERSION(2));
    reader.read(data.field3);
    TEST_ASSERT_TRUE(reader.checkHash());
    Data2 expected;
    TEST_ASSERT_TRUE((data.field1) == (expected.field1));
    TEST_ASSERT_TRUE((data.field2) == (expected.field2));
    TEST_ASSERT_TRUE((data.field4) == (reader.dataVersion() >= 2 ? expected.field4 : int8_t(0)));
    TEST_ASSERT_TRUE((data.field3) == (expected.field3));
}

static void readVersion3(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
    Data3 data {};
    data.field4 = 0;
    data.field5 = 0;
    reader.read(data.field1);
    reader.read(data.field2);
    reader.read(data.field4, VERSION(2));
    reader.read(data.field5, VERSION(3));
    reader.read(data.field3);
    TEST_ASSERT_TRUE(reader.checkHash());
    Data3 expected;
    TEST_ASSERT_TRUE((data.field1) == (expected.field1));
    TEST_ASSERT_TRUE((data.field2) == (expected.field2));
    TEST_ASSERT_TRUE((data.field4) == (reader.dataVersion() >= 2 ? expected.field4 : int8_t(0)));
    TEST_ASSERT_TRUE((data.field5) == (reader.dataVersion() >= 3 ? expected.field5 : int16_t(0)));
    TEST_ASSERT_TRUE((data.field3) == (expected.field3));
}

static void readVersion4(const void *buf, size_t len) {
    MemoryReader memoryReader(buf, len);
    VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 4);
    Data4 data {};
    data.field5 = 0;
    reader.read(data.field1);
    reader.read(data.field2);
    reader.skip<int8_t>(VERSION(2), VERSION(4));
    reader.read(data.field5, VERSION(3));
    reader.read(data.field3);
    TEST_ASSERT_TRUE(reader.checkHash());
    Data4 expected;
    TEST_ASSERT_TRUE((data.field1) == (expected.field1));
    TEST_ASSERT_TRUE((data.field2) == (expected.field2));
    TEST_ASSERT_TRUE((data.field5) == (reader.dataVersion() >= 3 ? expected.field5 : int16_t(0)));
    TEST_ASSERT_TRUE((data.field3) == (expected.field3));
}

static uint8_t buf[512];

static void clear() {
    std::memset(buf, 0, sizeof(buf));
}

// Scope:
    // - This test validates generic versioned serialization on an in-memory byte stream (buf).
    // - It does NOT test Project or Settings file formats.
    //
    // Serialized stream layout written by VersionedSerializedWriter:
    // [uint32_t dataVersion][field bytes for that dataVersion][uint32_t checksum]
    //
    // Schema evolution in this synthetic test model:
    // - field4 is introduced in data version 2
    // - field5 is introduced in data version 3
    // - field4 is removed again in data version 4

void test_01_reader_v1_decodes_data_version_1_stream() {
        // Guarantee: the original v1 layout is read correctly and checksum validation passes.
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion1(buf, sizeof(buf));
    
}

void test_02_reader_v2_decodes_data_version_1_and_2_streams() {
        // Guarantee: v2 reader keeps field4 default for v1 data and reads field4 for v2 data.
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion2(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion2(buf, sizeof(buf));
    
}

void test_03_reader_v3_decodes_data_version_1_through_3_streams() {
        // Guarantee: version-gated reads for field4/field5 behave correctly across v1-v3 inputs.
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));

        clear();
        writeVersion3(buf, sizeof(buf));
        readVersion3(buf, sizeof(buf));
    
}

void test_04_reader_v4_decodes_data_version_1_through_4_streams() {
        // Guarantee: removed field4 is skipped only for versions where it existed, and stream alignment stays correct.
        clear();
        writeVersion1(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion2(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion3(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));

        clear();
        writeVersion4(buf, sizeof(buf));
        readVersion4(buf, sizeof(buf));
    
}

void test_05_field4_remains_default_when_data_version_is_below_2() {
        // Guarantee: read(..., addedInVersion=2) does not consume bytes on v1 data and field4 keeps pre-set default.
        clear();
        writeVersion1(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 2);
        Data2 data {};
        data.field4 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(1)));
        TEST_ASSERT_TRUE((data.field4) == (int8_t(0)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_06_field4_is_read_from_stream_when_data_version_is_2() {
        // Guarantee: read(..., addedInVersion=2) consumes field4 bytes on v2 data and restores serialized value.
        clear();
        writeVersion2(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 2);
        Data2 data {};
        data.field4 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(2)));
        TEST_ASSERT_TRUE((data.field4) == (int8_t(-123)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_07_field5_remains_default_when_data_version_is_below_3() {
        // Guarantee: read(..., addedInVersion=3) does not consume bytes on v2 data and field5 keeps pre-set default.
        clear();
        writeVersion2(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
        Data3 data {};
        data.field4 = 0;
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(2)));
        TEST_ASSERT_TRUE((data.field5) == (int16_t(0)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_08_field5_is_read_from_stream_when_data_version_is_3() {
        // Guarantee: read(..., addedInVersion=3) consumes field5 bytes on v3 data and restores serialized value.
        clear();
        writeVersion3(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
        Data3 data {};
        data.field4 = 0;
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(3)));
        TEST_ASSERT_TRUE((data.field5) == (int16_t(-234)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_09_skip_consumes_removed_field4_bytes_for_data_versions_2_and_3() {
        // Guarantee: skip<int8_t>(2,4) advances stream position for historical layouts that still contain field4.
        clear();
        writeVersion2(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 4);
        Data4 data {};
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.skip<int8_t>(VERSION(2), VERSION(4));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(2)));
        TEST_ASSERT_TRUE((data.field5) == (int16_t(0)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_10_skip_does_nothing_for_removed_field4_at_data_version_4() {
        // Guarantee: skip<int8_t>(2,4) does not consume bytes once field4 is removed from the layout.
        clear();
        writeVersion4(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 4);
        Data4 data {};
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.skip<int8_t>(VERSION(2), VERSION(4));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_TRUE((reader.dataVersion()) == (uint32_t(4)));
        TEST_ASSERT_TRUE((data.field5) == (int16_t(-234)));
        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_11_checksum_validation_passes_for_unmodified_stream() {
        // Guarantee: checkHash() returns true when serialized bytes are read without tampering.
        clear();
        writeVersion3(buf, sizeof(buf));

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
        Data3 data {};
        data.field4 = 0;
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_TRUE(reader.checkHash());
    
}

void test_12_checksum_validation_fails_after_payload_corruption() {
        // Guarantee: checkHash() returns false when any payload byte changes after serialization.
        clear();
        writeVersion3(buf, sizeof(buf));
        buf[4] ^= 0x01;

        MemoryReader memoryReader(buf, sizeof(buf));
        VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) { memoryReader.read(data, len); }, 3);
        Data3 data {};
        data.field4 = 0;
        data.field5 = 0;
        reader.read(data.field1);
        reader.read(data.field2);
        reader.read(data.field4, VERSION(2));
        reader.read(data.field5, VERSION(3));
        reader.read(data.field3);

        TEST_ASSERT_FALSE(reader.checkHash());
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_reader_v1_decodes_data_version_1_stream);
    RUN_TEST(test_02_reader_v2_decodes_data_version_1_and_2_streams);
    RUN_TEST(test_03_reader_v3_decodes_data_version_1_through_3_streams);
    RUN_TEST(test_04_reader_v4_decodes_data_version_1_through_4_streams);
    RUN_TEST(test_05_field4_remains_default_when_data_version_is_below_2);
    RUN_TEST(test_06_field4_is_read_from_stream_when_data_version_is_2);
    RUN_TEST(test_07_field5_remains_default_when_data_version_is_below_3);
    RUN_TEST(test_08_field5_is_read_from_stream_when_data_version_is_3);
    RUN_TEST(test_09_skip_consumes_removed_field4_bytes_for_data_versions_2_and_3);
    RUN_TEST(test_10_skip_does_nothing_for_removed_field4_at_data_version_4);
    RUN_TEST(test_11_checksum_validation_passes_for_unmodified_stream);
    RUN_TEST(test_12_checksum_validation_fails_after_payload_corruption);
    return UNITY_END();
}

