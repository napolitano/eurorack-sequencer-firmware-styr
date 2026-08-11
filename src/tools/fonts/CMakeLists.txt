# Purpose: Defines the CMake build graph for the font conversion tooling.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(styr_font_tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(tool IN ITEMS
    glyphtext_to_bin
    c64font_dump_ascii
    c64font_to_bitmapfont
    bitmapfont_to_glyphtext
)
    add_executable(${tool} ${tool}.cpp)
    target_compile_options(${tool} PRIVATE -Wall -Wextra -pedantic)
endforeach()

find_package(Curses)
if(CURSES_FOUND)
    add_executable(glyphtext_editor glyphtext_editor.cpp)
    target_compile_options(glyphtext_editor PRIVATE -Wall -Wextra -pedantic)
    target_include_directories(glyphtext_editor PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(glyphtext_editor PRIVATE ${CURSES_LIBRARIES})
else()
    message(STATUS "Curses not found; glyphtext_editor will not be built")
endif()
