/**
 * @file Debug.h
 * @brief Declares the Debug component used by the bootloader.
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

#include "Config.h"
#include "Format.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ENABLE_DEBUG
# define DBG(_fmt_, ...) printf(_fmt_ "\n", ##__VA_ARGS__)
# define ASSERT_STR_HELPER(x) #x
# define ASSERT_STR(x) ASSERT_STR_HELPER(x)
# define ASSERT(_cond_, _fmt_, ...)                                                             \
    if (!(_cond_)) {                                                                            \
        printf("ASSERT in " __FILE__ ":" ASSERT_STR(__LINE__) "\n" _fmt_ "\n", ##__VA_ARGS__);  \
        while (1) {};                                                                           \
    }
#else // CONFIG_ENABLE_DEBUG
# define DBG(_fmt_, ...)
# define ASSERT(_cond_, _fmt_, ...)
#endif // CONFIG_ENABLE_DEBUG

#ifdef __cplusplus
}
#endif
