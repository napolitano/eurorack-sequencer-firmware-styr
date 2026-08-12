/**
 * @file TestBootloaderFormat.cpp
 * @brief Unity regression tests for the compact fixed-size bootloader formatter.
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

#include "Console.h"
#include "Format.h"

#include <climits>
#include <cstdint>
#include <cstring>

namespace {

char consoleBuffer[512];
std::size_t consoleLength = 0;

using SnprintfFunction = int (*)(char *, std::size_t, const char *, ...);
using PrintfFunction = int (*)(const char *, ...);

SnprintfFunction formatToBuffer = &::bootSnprintf;
PrintfFunction formatToConsole = &::bootPrintf;

void resetConsole() {
    consoleLength = 0;
    consoleBuffer[0] = '\0';
}

} // namespace

// Host stub for the bootloader UART console. Preserve the CRLF behavior of the
// production Console::write(char) implementation so bootPrintf() is tested
// through the same character-level contract used on hardware.
void Console::write(char c) {
    if (c == '\n' && consoleLength + 1 < sizeof(consoleBuffer)) {
        consoleBuffer[consoleLength++] = '\r';
    }
    if (consoleLength + 1 < sizeof(consoleBuffer)) {
        consoleBuffer[consoleLength++] = c;
    }
    consoleBuffer[consoleLength] = '\0';
}

void setUp() {
    resetConsole();
}

void tearDown() {}

void test_plain_percent_and_string_formats() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(10, formatToBuffer(buffer, sizeof(buffer), "plain text"));
    TEST_ASSERT_EQUAL_STRING("plain text", buffer);

    TEST_ASSERT_EQUAL_INT(4, formatToBuffer(buffer, sizeof(buffer), "100%%"));
    TEST_ASSERT_EQUAL_STRING("100%", buffer);

    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(buffer, sizeof(buffer), "[%s]", "styr"));
    TEST_ASSERT_EQUAL_STRING("[styr]", buffer);

    const char *nullString = nullptr;
    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(buffer, sizeof(buffer), "%s", nullString));
    TEST_ASSERT_EQUAL_STRING("(null)", buffer);
}

void test_signed_decimal_formats() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(5, formatToBuffer(buffer, sizeof(buffer), "%d", 12345));
    TEST_ASSERT_EQUAL_STRING("12345", buffer);

    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(buffer, sizeof(buffer), "%d", -12345));
    TEST_ASSERT_EQUAL_STRING("-12345", buffer);

    TEST_ASSERT_EQUAL_INT(11, formatToBuffer(buffer, sizeof(buffer), "%d", INT32_MIN));
    TEST_ASSERT_EQUAL_STRING("-2147483648", buffer);
}

void test_unsigned_and_hex_formats() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(10, formatToBuffer(buffer, sizeof(buffer), "%u", UINT32_MAX));
    TEST_ASSERT_EQUAL_STRING("4294967295", buffer);

    TEST_ASSERT_EQUAL_INT(8, formatToBuffer(buffer, sizeof(buffer), "%x", 0xdeadbeefu));
    TEST_ASSERT_EQUAL_STRING("deadbeef", buffer);
}

void test_width_and_zero_padding() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(5, formatToBuffer(buffer, sizeof(buffer), "%5u", 42u));
    TEST_ASSERT_EQUAL_STRING("   42", buffer);

    TEST_ASSERT_EQUAL_INT(8, formatToBuffer(buffer, sizeof(buffer), "%08x", 0x1a2bu));
    TEST_ASSERT_EQUAL_STRING("00001a2b", buffer);

    TEST_ASSERT_EQUAL_INT(5, formatToBuffer(buffer, sizeof(buffer), "%05d", -12));
    TEST_ASSERT_EQUAL_STRING("-0012", buffer);
}

void test_long_formats_used_by_bootloader() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(buffer, sizeof(buffer), "%lu", 983040ul));
    TEST_ASSERT_EQUAL_STRING("983040", buffer);

    TEST_ASSERT_EQUAL_INT(10, formatToBuffer(buffer, sizeof(buffer), "0x%08lx", 0x08010000ul));
    TEST_ASSERT_EQUAL_STRING("0x08010000", buffer);
}

void test_real_bootloader_format_strings() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(
        14,
        formatToBuffer(buffer, sizeof(buffer), "%s (%d.%d.%d)", "styr", 1, 2, 345));
    TEST_ASSERT_EQUAL_STRING("styr (1.2.345)", buffer);

    TEST_ASSERT_EQUAL_INT(
        19,
        formatToBuffer(buffer, sizeof(buffer), "verifying image %d%%", 73));
    TEST_ASSERT_EQUAL_STRING("verifying image 73%", buffer);
}

void test_exact_fit_and_truncation_follow_snprintf_contract() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(3, formatToBuffer(buffer, 4, "abc"));
    TEST_ASSERT_EQUAL_STRING("abc", buffer);

    std::memset(buffer, 'X', sizeof(buffer));
    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(buffer, 4, "abcdef"));
    TEST_ASSERT_EQUAL_STRING("abc", buffer);
}

void test_size_one_and_size_zero_are_bounded() {
    char buffer[2] = {'X', 'Y'};

    TEST_ASSERT_EQUAL_INT(3, formatToBuffer(buffer, 1, "abc"));
    TEST_ASSERT_EQUAL_STRING("", buffer);
    TEST_ASSERT_EQUAL_CHAR('Y', buffer[1]);

    TEST_ASSERT_EQUAL_INT(6, formatToBuffer(nullptr, 0, "abcdef"));
}

void test_unsupported_and_trailing_percent_are_literal() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(2, formatToBuffer(buffer, sizeof(buffer), "%q", 1));
    TEST_ASSERT_EQUAL_STRING("%q", buffer);

    TEST_ASSERT_EQUAL_INT(4, formatToBuffer(buffer, sizeof(buffer), "abc%"));
    TEST_ASSERT_EQUAL_STRING("abc%", buffer);
}


void test_zero_width_and_large_width_remain_bounded() {
    char buffer[8];

    TEST_ASSERT_EQUAL_INT(1, formatToBuffer(buffer, sizeof(buffer), "%0u", 7u));
    TEST_ASSERT_EQUAL_STRING("7", buffer);

    TEST_ASSERT_EQUAL_INT(20, formatToBuffer(buffer, sizeof(buffer), "%020u", 42u));
    TEST_ASSERT_EQUAL_STRING("0000000", buffer);
}

void test_multiple_conversions_preserve_argument_order() {
    char buffer[128];

    TEST_ASSERT_EQUAL_INT(23, formatToBuffer(
        buffer, sizeof(buffer), "%s:%d:%u:%02x:0x%08lx",
        "styr", -7, 9u, 0x0au, 0x08010000ul));
    TEST_ASSERT_EQUAL_STRING("styr:-7:9:0a:0x08010000", buffer);
}

void test_console_return_value_counts_lf_not_inserted_cr() {
    TEST_ASSERT_EQUAL_INT(4, formatToConsole("a\nb\n"));
    TEST_ASSERT_EQUAL_STRING("a\r\nb\r\n", consoleBuffer);
}

void test_boot_printf_writes_through_console_contract() {
    TEST_ASSERT_EQUAL_INT(
        23,
        formatToConsole("md5:%02x addr:0x%08lx\n", 0x0a, 0x08010000ul));
    TEST_ASSERT_EQUAL_STRING("md5:0a addr:0x08010000\r\n", consoleBuffer);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_plain_percent_and_string_formats);
    RUN_TEST(test_signed_decimal_formats);
    RUN_TEST(test_unsigned_and_hex_formats);
    RUN_TEST(test_width_and_zero_padding);
    RUN_TEST(test_long_formats_used_by_bootloader);
    RUN_TEST(test_real_bootloader_format_strings);
    RUN_TEST(test_exact_fit_and_truncation_follow_snprintf_contract);
    RUN_TEST(test_size_one_and_size_zero_are_bounded);
    RUN_TEST(test_unsupported_and_trailing_percent_are_literal);
    RUN_TEST(test_zero_width_and_large_width_remain_bounded);
    RUN_TEST(test_multiple_conversions_preserve_argument_order);
    RUN_TEST(test_console_return_value_counts_lf_not_inserted_cr);
    RUN_TEST(test_boot_printf_writes_through_console_contract);
    return UNITY_END();
}
