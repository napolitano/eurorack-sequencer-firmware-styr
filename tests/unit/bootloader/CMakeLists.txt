# Purpose: Defines host-side tests for bootloader-critical pure logic.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

add_executable(TestBootloaderFormat
    TestBootloaderFormat.cpp
    ${STYR_ROOT}/src/bootloader/Format.cpp
)
target_include_directories(TestBootloaderFormat PRIVATE ${STYR_ROOT}/src/bootloader)
platform_postprocess_executable(TestBootloaderFormat)
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME TestBootloaderFormat COMMAND TestBootloaderFormat)
endif()

add_executable(TestBootloaderUpdatePolicy TestBootloaderUpdatePolicy.cpp)
target_include_directories(TestBootloaderUpdatePolicy PRIVATE ${STYR_ROOT}/src/bootloader)
platform_postprocess_executable(TestBootloaderUpdatePolicy)
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME TestBootloaderUpdatePolicy COMMAND TestBootloaderUpdatePolicy)
endif()

add_executable(TestBootloaderFlashLayout TestBootloaderFlashLayout.cpp)
target_include_directories(TestBootloaderFlashLayout PRIVATE ${STYR_ROOT}/src/bootloader)
platform_postprocess_executable(TestBootloaderFlashLayout)
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME TestBootloaderFlashLayout COMMAND TestBootloaderFlashLayout)
endif()

add_executable(TestBootloaderMd5
    TestBootloaderMd5.cpp
    ${STYR_ROOT}/third_party/embedded/bootloader/md5/MD5.cpp
)
target_include_directories(TestBootloaderMd5 PRIVATE ${STYR_ROOT}/third_party/embedded/bootloader/md5)
platform_postprocess_executable(TestBootloaderMd5)
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME TestBootloaderMd5 COMMAND TestBootloaderMd5)
endif()

add_executable(TestBootloaderUpdateFile
    TestBootloaderUpdateFile.cpp
    ${STYR_ROOT}/src/bootloader/UpdateFile.cpp
    ${STYR_ROOT}/src/bootloader/Format.cpp
)
target_include_directories(TestBootloaderUpdateFile PRIVATE
    ${STYR_ROOT}/src/bootloader
    ${STYR_ROOT}/third_party/embedded/bootloader
)
platform_postprocess_executable(TestBootloaderUpdateFile)
if(NOT CMAKE_CROSSCOMPILING)
    add_test(NAME TestBootloaderUpdateFile COMMAND TestBootloaderUpdateFile)
endif()
