/**
 * @file TestBootloaderMd5.cpp
 * @brief Unity tests for the MD5 implementation used by UPDATE.DAT integrity checks.
 *
 * @author Axel Napolitano — Styr test integration
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt. MD5 implementation retains its upstream public-domain notice.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#include <unity.h>

#include "MD5.h"

#include <cstddef>
#include <cstring>
#include <vector>

namespace {

unsigned nibble(char c) {
    if (c >= '0' && c <= '9') return static_cast<unsigned>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<unsigned>(c - 'a' + 10);
    return 0xffu;
}

bool matches(const unsigned char digest[16], const char *hex) {
    for (std::size_t i = 0; i < 16; ++i) {
        const unsigned hi = nibble(hex[i * 2]);
        const unsigned lo = nibble(hex[i * 2 + 1]);
        if (hi > 15 || lo > 15 ||
            digest[i] != static_cast<unsigned char>((hi << 4) | lo)) {
            return false;
        }
    }
    return true;
}

void assertVector(const void *message, std::size_t length, const char *expected) {
    MD5 md5;
    md5.update(message, static_cast<unsigned long>(length));
    MD5::Sum sum;
    md5.finish(sum);
    TEST_ASSERT_TRUE(matches(sum, expected));
}

void assertStringVector(const char *message, const char *expected) {
    assertVector(message, std::strlen(message), expected);
}

} // namespace

void setUp() {}
void tearDown() {}

void test_rfc1321_reference_vectors() {
    assertStringVector("", "d41d8cd98f00b204e9800998ecf8427e");
    assertStringVector("a", "0cc175b9c0f1b6a831c399e269772661");
    assertStringVector("abc", "900150983cd24fb0d6963f7d28e17f72");
    assertStringVector("message digest", "f96b697d7cb7938d525a2f31aaf161d0");
    assertStringVector("abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b");
    assertStringVector(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "d174ab98d277d9f5a5611c2c9f419d9f");
    assertStringVector(
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
        "57edf4a22be3c955ac49da2e2107b67a");
}

void test_padding_boundaries_around_single_md5_block() {
    const struct {
        std::size_t length;
        const char *digest;
    } vectors[] = {
        {55, "ef1772b6dff9a122358552954ad0df65"},
        {56, "3b0c8ac703f828b04c6c197006d17218"},
        {63, "b06521f39153d618550606be297466d5"},
        {64, "014842d480b571495a4a0363793f7367"},
        {65, "c743a45e0d2e6a95cb859adae0248435"},
    };

    std::vector<unsigned char> data(65, static_cast<unsigned char>('a'));
    for (const auto &vector : vectors) {
        assertVector(data.data(), vector.length, vector.digest);
    }
}

void test_incremental_updates_match_bootloader_read_loop_usage() {
    MD5 chunked;
    chunked.update("The quick ", 10);
    chunked.update("brown fox ", 10);
    chunked.update("jumps over the lazy dog", 23);
    MD5::Sum sum;
    chunked.finish(sum);
    TEST_ASSERT_TRUE(matches(sum, "9e107d9d372bb6826bd81d3542a419d6"));
}

void test_1024_byte_bootloader_chunk_has_known_digest() {
    std::vector<unsigned char> data(1024, static_cast<unsigned char>('a'));
    assertVector(data.data(), data.size(), "c9a34cfc85d982698c6ac89f76071abd");
}

void test_data_crossing_bootloader_chunk_boundary_has_known_digest() {
    std::vector<unsigned char> data(1025, static_cast<unsigned char>('a'));
    assertVector(data.data(), data.size(), "ae187e1febee2a150b64849c32d566ca");
}

void test_same_payload_digest_is_independent_of_update_chunking() {
    std::vector<unsigned char> data(4097);
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<unsigned char>((i * 37u + 11u) & 0xffu);
    }

    MD5 oneShot;
    oneShot.update(data.data(), static_cast<unsigned long>(data.size()));
    MD5::Sum expected;
    oneShot.finish(expected);

    MD5 chunked;
    std::size_t offset = 0;
    const std::size_t chunks[] = {1, 3, 64, 255, 1024, 7, 511};
    std::size_t chunkIndex = 0;
    while (offset < data.size()) {
        std::size_t count = chunks[chunkIndex++ % (sizeof(chunks) / sizeof(chunks[0]))];
        if (count > data.size() - offset) {
            count = data.size() - offset;
        }
        chunked.update(data.data() + offset, static_cast<unsigned long>(count));
        offset += count;
    }
    MD5::Sum actual;
    chunked.finish(actual);

    TEST_ASSERT_EQUAL_MEMORY(expected, actual, sizeof(expected));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_rfc1321_reference_vectors);
    RUN_TEST(test_padding_boundaries_around_single_md5_block);
    RUN_TEST(test_incremental_updates_match_bootloader_read_loop_usage);
    RUN_TEST(test_1024_byte_bootloader_chunk_has_known_digest);
    RUN_TEST(test_data_crossing_bootloader_chunk_boundary_has_known_digest);
    RUN_TEST(test_same_payload_digest_is_independent_of_update_chunking);
    return UNITY_END();
}
