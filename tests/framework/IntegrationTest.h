/**
 * @file IntegrationTest.h
 * @brief Declares the IntegrationTest component used by the test framework.
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
#pragma once

#include "test/IntegrationTestRunner.h"
#include "Timer.h"

class IntegrationTest {
public:
    virtual ~IntegrationTest() {}
    virtual void init() {}
    virtual void once() {}
    virtual void update() {}
};

#define EXPECT(_cond_, _fmt_, ...) \
    INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ##__VA_ARGS__)

#define INTEGRATION_TEST(_class_, _name_, _interactive_) \
    INTEGRATION_TEST_RUNNER(_class_, _name_, _interactive_)
