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
#include "UnitTest.h"

#include "core/utils/StringUtils.h"

UNIT_TEST("StringUtils") {

    CASE("copy") {
        char buf[4];

        buf[0] = '\1';
        StringUtils::copy(buf, "", 0);
        expectEqual(buf[0], '\1');

        buf[0] = '\1';
        StringUtils::copy(buf, "", 1);
        expectEqual(buf, "");

        buf[0] = '\1';
        StringUtils::copy(buf, "", sizeof(buf));
        expectEqual(buf, "");

        StringUtils::copy(buf, "a", sizeof(buf));
        expectEqual(buf, "a");

        StringUtils::copy(buf, "abc", sizeof(buf));
        expectEqual(buf, "abc");

        StringUtils::copy(buf, "test", sizeof(buf));
        expectEqual(buf, "tes");
    }

}
