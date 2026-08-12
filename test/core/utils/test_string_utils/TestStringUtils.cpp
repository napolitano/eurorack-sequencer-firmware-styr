/**
 * @file TestStringUtils.cpp
 * @brief Implements the TestStringUtils component used by the unit test.
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

#include "core/utils/StringUtils.h"

void test_01_copy() {
        char buf[4];

        buf[0] = '\1';
        StringUtils::copy(buf, "", 0);
        TEST_ASSERT_TRUE((buf[0]) == ('\1'));

        buf[0] = '\1';
        StringUtils::copy(buf, "", 1);
        TEST_ASSERT_EQUAL_STRING("", buf);

        buf[0] = '\1';
        StringUtils::copy(buf, "", sizeof(buf));
        TEST_ASSERT_EQUAL_STRING("", buf);

        StringUtils::copy(buf, "a", sizeof(buf));
        TEST_ASSERT_EQUAL_STRING("a", buf);

        StringUtils::copy(buf, "abc", sizeof(buf));
        TEST_ASSERT_EQUAL_STRING("abc", buf);

        StringUtils::copy(buf, "test", sizeof(buf));
        TEST_ASSERT_EQUAL_STRING("tes", buf);
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_copy);
    return UNITY_END();
}

