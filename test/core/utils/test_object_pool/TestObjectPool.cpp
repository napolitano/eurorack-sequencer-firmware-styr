/**
 * @file TestObjectPool.cpp
 * @brief Implements the TestObjectPool component used by the unit test.
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

#include "core/utils/ObjectPool.h"

void test_01_allocate_release_all() {
        ObjectPool<int, 4> pool;

        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(4)));

        int *items[4];

        for (size_t i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            TEST_ASSERT_TRUE(items[i] != nullptr);
            TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
            TEST_ASSERT_TRUE((pool.size()) == (size_t(3 - i)));
            *items[i] = i;
        }

        for (int i = 0; i < 4; ++i) {
            TEST_ASSERT_TRUE((*items[i]) == (i));
        }

        for (size_t i = 0; i < 4; ++i) {
            pool.release(items[i]);
            TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
            TEST_ASSERT_TRUE((pool.size()) == (size_t(i + 1)));
        }
    
}

void test_02_allocate_release_reuse() {
        ObjectPool<int, 4> pool;

        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(4)));

        int *items[4];

        for (size_t i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            TEST_ASSERT_TRUE(items[i] != nullptr);
            TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
            TEST_ASSERT_TRUE((pool.size()) == (size_t(3 - i)));
            *items[i] = i;
        }

        for (int i = 0; i < 4; ++i) {
            TEST_ASSERT_TRUE((*items[i]) == (i));
        }

        pool.release(items[0]);
        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(1)));

        pool.release(items[3]);
        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(2)));

        items[0] = pool.allocate();
        TEST_ASSERT_TRUE(items[0] != nullptr && items[0] != items[1] && items[0] != items[2]);
        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(1)));

        items[3] = pool.allocate();
        TEST_ASSERT_TRUE(items[3] != nullptr && items[3] != items[0] && items[3] != items[1] && items[3] != items[2]);
        TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
        TEST_ASSERT_TRUE((pool.size()) == (size_t(0)));

        for (size_t i = 0; i < 4; ++i) {
            pool.release(items[i]);
            TEST_ASSERT_TRUE((pool.capacity()) == (size_t(4)));
            TEST_ASSERT_TRUE((pool.size()) == (size_t(i + 1)));
        }
    
}

void test_03_foreach() {
        ObjectPool<int, 4> pool;
        int *items[4];

        for (int i = 0; i < 4; ++i) {
            items[i] = pool.allocate();
            *items[i] = 1 << i;
        }

        pool.release(items[0]);
        pool.release(items[3]);

        int sum = 0;
        pool.forEach([&sum] (int i) {
            sum += i;
        });
        TEST_ASSERT_TRUE((sum) == (6));
    
}
void setUp() {}

void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_01_allocate_release_all);
    RUN_TEST(test_02_allocate_release_reuse);
    RUN_TEST(test_03_foreach);
    return UNITY_END();
}

