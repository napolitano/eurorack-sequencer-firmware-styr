/**
 * @file TestBootloaderUpdatePolicy.cpp
 * @brief Unity tests for UPDATE.DAT size and application-partition invariants.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include <unity.h>

#include "UpdateImagePolicy.h"

#include <cstddef>
#include <cstdint>
#include <limits>

void setUp() {}
void tearDown() {}

void test_invalid_small_files_are_rejected_without_modifying_output() {
    std::size_t payload = 0x55aa55aa;

    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(0, payload));
    TEST_ASSERT_EQUAL_size_t(0x55aa55aa, payload);

    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(BootloaderUpdate::ChecksumSize - 1, payload));
    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(BootloaderUpdate::ChecksumSize, payload));
    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize - 1,
        payload));
    TEST_ASSERT_EQUAL_size_t(0x55aa55aa, payload);
}

void test_minimum_and_maximum_payload_boundaries_are_accepted() {
    std::size_t payload = 0;

    TEST_ASSERT_TRUE(BootloaderUpdate::payloadSize(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MinimumPayloadSize,
        payload));
    TEST_ASSERT_EQUAL_size_t(BootloaderUpdate::MinimumPayloadSize, payload);

    TEST_ASSERT_TRUE(BootloaderUpdate::payloadSize(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize,
        payload));
    TEST_ASSERT_EQUAL_size_t(BootloaderUpdate::MaximumPayloadSize, payload);
}

void test_oversize_and_size_t_extremes_are_rejected_without_overflow() {
    std::size_t payload = 0x12345678;

    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(
        BootloaderUpdate::ChecksumSize + BootloaderUpdate::MaximumPayloadSize + 1,
        payload));
    TEST_ASSERT_EQUAL_size_t(0x12345678, payload);

    TEST_ASSERT_FALSE(BootloaderUpdate::payloadSize(
        std::numeric_limits<std::size_t>::max(), payload));
    TEST_ASSERT_EQUAL_size_t(0x12345678, payload);
}

void test_flash_word_rounding_is_exact_for_partial_words() {
    TEST_ASSERT_EQUAL_size_t(0, BootloaderUpdate::programWordCount(0));
    TEST_ASSERT_EQUAL_size_t(1, BootloaderUpdate::programWordCount(1));
    TEST_ASSERT_EQUAL_size_t(1, BootloaderUpdate::programWordCount(2));
    TEST_ASSERT_EQUAL_size_t(1, BootloaderUpdate::programWordCount(3));
    TEST_ASSERT_EQUAL_size_t(1, BootloaderUpdate::programWordCount(4));
    TEST_ASSERT_EQUAL_size_t(2, BootloaderUpdate::programWordCount(5));
    TEST_ASSERT_EQUAL_size_t(256, BootloaderUpdate::programWordCount(1024));

    TEST_ASSERT_EQUAL_size_t(0, BootloaderUpdate::programmedSize(0));
    TEST_ASSERT_EQUAL_size_t(4, BootloaderUpdate::programmedSize(1));
    TEST_ASSERT_EQUAL_size_t(4, BootloaderUpdate::programmedSize(4));
    TEST_ASSERT_EQUAL_size_t(8, BootloaderUpdate::programmedSize(5));
}

void test_every_valid_tail_size_near_partition_end_stays_inside_partition() {
    for (std::size_t delta = 0; delta < 4; ++delta) {
        const std::size_t payload = BootloaderUpdate::MaximumPayloadSize - delta;
        TEST_ASSERT_LESS_OR_EQUAL_UINT32(
            static_cast<uint32_t>(BootloaderUpdate::MaximumPayloadSize),
            static_cast<uint32_t>(BootloaderUpdate::programmedSize(payload)));
    }
}

void test_version_tag_and_policy_constants_match_flash_contract() {
    VersionTag validTag = {};
    validTag.magic = CONFIG_VERSION_TAG_MAGIC;
    TEST_ASSERT_TRUE(validTag.isValid());
    validTag.magic ^= 1u;
    TEST_ASSERT_FALSE(validTag.isValid());

    TEST_ASSERT_EQUAL_size_t(16, BootloaderUpdate::ChecksumSize);
    TEST_ASSERT_EQUAL_size_t(4, BootloaderUpdate::FlashWordSize);
    TEST_ASSERT_EQUAL_size_t(CONFIG_APPLICATION_SIZE, BootloaderUpdate::MaximumPayloadSize);
    TEST_ASSERT_EQUAL_size_t(
        CONFIG_VERSION_TAG_OFFSET + sizeof(VersionTag),
        BootloaderUpdate::MinimumPayloadSize);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_invalid_small_files_are_rejected_without_modifying_output);
    RUN_TEST(test_minimum_and_maximum_payload_boundaries_are_accepted);
    RUN_TEST(test_oversize_and_size_t_extremes_are_rejected_without_overflow);
    RUN_TEST(test_flash_word_rounding_is_exact_for_partial_words);
    RUN_TEST(test_every_valid_tail_size_near_partition_end_stays_inside_partition);
    RUN_TEST(test_version_tag_and_policy_constants_match_flash_contract);
    return UNITY_END();
}
