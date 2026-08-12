/**
 * @file TestBootloaderFormat.cpp
 * @brief Regression tests for the compact fixed-size bootloader formatter.
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
#include "Format.h"
#include "Console.h"

#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace {

char consoleBuffer[512];
std::size_t consoleLength = 0;
int failures = 0;

using SnprintfFunction = int (*)(char *, std::size_t, const char *, ...);
using PrintfFunction = int (*)(const char *, ...);

SnprintfFunction formatToBuffer = &::bootSnprintf;
PrintfFunction formatToConsole = &::bootPrintf;

void resetConsole() {
    consoleLength = 0;
    consoleBuffer[0] = '\0';
}

void fail(const char *caseName, const char *detail) {
    ++failures;
    std::cerr << "FAILED: " << caseName << ": " << detail << '\n';
}

void expectString(const char *caseName, const char *actual, const char *expected) {
    if (std::strcmp(actual, expected) != 0) {
        ++failures;
        std::cerr << "FAILED: " << caseName << ": expected [" << expected
                  << "] got [" << actual << "]\n";
    }
}

void expectInt(const char *caseName, int actual, int expected) {
    if (actual != expected) {
        ++failures;
        std::cerr << "FAILED: " << caseName << ": expected " << expected
                  << " got " << actual << '\n';
    }
}

} // namespace

// Host stub for the bootloader UART console. Preserve the CRLF behavior of the
// production Console::write(char) implementation so printf() is tested through
// the same character-level contract used on hardware.
void Console::write(char c) {
    if (c == '\n' && consoleLength + 1 < sizeof(consoleBuffer)) {
        consoleBuffer[consoleLength++] = '\r';
    }
    if (consoleLength + 1 < sizeof(consoleBuffer)) {
        consoleBuffer[consoleLength++] = c;
    }
    consoleBuffer[consoleLength] = '\0';
}

int main() {
    char buffer[128];

    expectInt("plain return", formatToBuffer(buffer, sizeof(buffer), "plain text"), 10);
    expectString("plain", buffer, "plain text");

    expectInt("percent return", formatToBuffer(buffer, sizeof(buffer), "100%%"), 4);
    expectString("percent", buffer, "100%");

    expectInt("string return", formatToBuffer(buffer, sizeof(buffer), "[%s]", "styr"), 6);
    expectString("string", buffer, "[styr]");

    const char *nullString = nullptr;
    expectInt("null string return", formatToBuffer(buffer, sizeof(buffer), "%s", nullString), 6);
    expectString("null string", buffer, "(null)");

    expectInt("signed positive return", formatToBuffer(buffer, sizeof(buffer), "%d", 12345), 5);
    expectString("signed positive", buffer, "12345");

    expectInt("signed negative return", formatToBuffer(buffer, sizeof(buffer), "%d", -12345), 6);
    expectString("signed negative", buffer, "-12345");

    expectInt("int32 min return", formatToBuffer(buffer, sizeof(buffer), "%d", INT32_MIN), 11);
    expectString("int32 min", buffer, "-2147483648");

    expectInt("unsigned max return", formatToBuffer(buffer, sizeof(buffer), "%u", UINT32_MAX), 10);
    expectString("unsigned max", buffer, "4294967295");

    expectInt("hex return", formatToBuffer(buffer, sizeof(buffer), "%x", 0xdeadbeefu), 8);
    expectString("hex", buffer, "deadbeef");

    expectInt("space width return", formatToBuffer(buffer, sizeof(buffer), "%5u", 42u), 5);
    expectString("space width", buffer, "   42");

    expectInt("zero width return", formatToBuffer(buffer, sizeof(buffer), "%08x", 0x1a2bu), 8);
    expectString("zero width", buffer, "00001a2b");

    expectInt("negative zero width return", formatToBuffer(buffer, sizeof(buffer), "%05d", -12), 5);
    expectString("negative zero width", buffer, "-0012");

    expectInt("long unsigned return", formatToBuffer(buffer, sizeof(buffer), "%lu", 983040ul), 6);
    expectString("long unsigned", buffer, "983040");

    expectInt("long hex return", formatToBuffer(buffer, sizeof(buffer), "0x%08lx", 0x08010000ul), 10);
    expectString("long hex", buffer, "0x08010000");

    expectInt("version format return",
              formatToBuffer(buffer, sizeof(buffer), "%s (%d.%d.%d)", "styr", 1, 2, 345),
              14);
    expectString("version format", buffer, "styr (1.2.345)");

    expectInt("progress format return", formatToBuffer(buffer, sizeof(buffer), "verifying image %d%%", 73), 19);
    expectString("progress format", buffer, "verifying image 73%");

    expectInt("exact fit return", formatToBuffer(buffer, 4, "abc"), 3);
    expectString("exact fit", buffer, "abc");

    std::memset(buffer, 'X', sizeof(buffer));
    expectInt("truncation return", formatToBuffer(buffer, 4, "abcdef"), 6);
    expectString("truncation", buffer, "abc");

    buffer[0] = 'X';
    buffer[1] = 'Y';
    expectInt("size one return", formatToBuffer(buffer, 1, "abc"), 3);
    expectString("size one", buffer, "");
    if (buffer[1] != 'Y') {
        fail("size one bounds", "formatter wrote beyond the one-byte buffer");
    }

    expectInt("size zero return", formatToBuffer(nullptr, 0, "abcdef"), 6);

    expectInt("unsupported conversion return", formatToBuffer(buffer, sizeof(buffer), "%q", 1), 2);
    expectString("unsupported conversion", buffer, "%q");

    expectInt("trailing percent return", formatToBuffer(buffer, sizeof(buffer), "abc%"), 4);
    expectString("trailing percent", buffer, "abc%");

    resetConsole();
    expectInt("console return",
              formatToConsole("md5:%02x addr:0x%08lx\n", 0x0a, 0x08010000ul),
              23);
    expectString("console output", consoleBuffer, "md5:0a addr:0x08010000\r\n");

    if (failures != 0) {
        std::cerr << failures << " bootloader formatter test(s) failed\n";
        return 1;
    }

    std::cout << "Bootloader formatter: all cases passed\n";
    return 0;
}
