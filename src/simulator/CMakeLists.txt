# Purpose: Defines the CMake build graph for the desktop simulator.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(styr_simulator LANGUAGES C CXX)

set(STYR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
get_filename_component(STYR_ROOT "${STYR_ROOT}" ABSOLUTE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${STYR_ROOT}/third_party/simulator/cmake")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(STYR_BUILD_TESTS "Build native unit and integration tests" ON)
option(STYR_SIM_ENABLE_PYTHON "Build Python simulator bindings" OFF)
set(STYR_DOC_SCREENSHOT_SCALE "3" CACHE STRING
    "Integer nearest-neighbour scale for generated manual LCD screenshots")
if(NOT STYR_DOC_SCREENSHOT_SCALE MATCHES "^[1-8]$")
    message(FATAL_ERROR "STYR_DOC_SCREENSHOT_SCALE must be an integer from 1 to 8")
endif()

set(STYR_SHARED_DIR "${STYR_ROOT}/src/shared")
set(STYR_SEQUENCER_DIR "${STYR_ROOT}/src/sequencer")
set(STYR_TEST_FRAMEWORK_DIR "${STYR_ROOT}/tests/framework")
set(STYR_THIRD_PARTY_COMMON_DIR "${STYR_ROOT}/third_party/common")
set(STYR_THIRD_PARTY_SIM_DIR "${STYR_ROOT}/third_party/simulator")
set(STYR_SIM_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platform")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/StyrDependencies.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(STYR_EMSCRIPTEN_FLAGS "-s USE_SDL=2 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    find_package(OpenGL REQUIRED)
    if(WIN32)
        # Native Windows host dependencies deliberately come from one MSYS2
        # UCRT64 prefix.  Resolve SDL2 and GLEW directly so an unrelated CMake
        # installation cannot inject SDL2main/-mwindows into library consumers
        # or use a different FindGLEW implementation.
        set(STYR_UCRT64_PREFIX "C:/msys64/ucrt64" CACHE PATH
            "MSYS2 UCRT64 prefix used by the native Windows simulator")

        find_path(STYR_SDL2_INCLUDE_DIR
            NAMES SDL.h
            HINTS "${STYR_UCRT64_PREFIX}/include/SDL2"
            NO_DEFAULT_PATH
        )
        find_library(STYR_SDL2_LIBRARY
            NAMES SDL2
            HINTS "${STYR_UCRT64_PREFIX}/lib"
            NO_DEFAULT_PATH
        )
        if(NOT STYR_SDL2_INCLUDE_DIR OR NOT STYR_SDL2_LIBRARY)
            message(FATAL_ERROR
                "SDL2 was not found in ${STYR_UCRT64_PREFIX}. "
                "Install the MSYS2 UCRT64 package with: "
                "C:/msys64/usr/bin/pacman.exe -S --needed mingw-w64-ucrt-x86_64-SDL2"
            )
        endif()

        find_path(STYR_GLEW_INCLUDE_DIR
            NAMES GL/glew.h
            HINTS "${STYR_UCRT64_PREFIX}/include"
            NO_DEFAULT_PATH
        )
        find_library(STYR_GLEW_LIBRARY
            NAMES glew32 glew32s GLEW glew
            HINTS "${STYR_UCRT64_PREFIX}/lib"
            NO_DEFAULT_PATH
        )
        if(NOT STYR_GLEW_INCLUDE_DIR OR NOT STYR_GLEW_LIBRARY)
            message(FATAL_ERROR
                "GLEW was not found in ${STYR_UCRT64_PREFIX}. "
                "Install the MSYS2 UCRT64 package with: "
                "C:/msys64/usr/bin/pacman.exe -S --needed mingw-w64-ucrt-x86_64-glew"
            )
        endif()

        add_library(styr_sdl2 UNKNOWN IMPORTED)
        set_target_properties(styr_sdl2 PROPERTIES
            IMPORTED_LOCATION "${STYR_SDL2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${STYR_SDL2_INCLUDE_DIR}"
        )
        add_library(styr_glew UNKNOWN IMPORTED)
        set_target_properties(styr_glew PROPERTIES
            IMPORTED_LOCATION "${STYR_GLEW_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${STYR_GLEW_INCLUDE_DIR}"
        )

        # Stage the exact UCRT64 runtime beside every native executable.
        # This prevents Windows from resolving an incompatible libstdc++ or
        # other MinGW runtime from an unrelated toolchain on PATH when the
        # simulator is launched from Explorer or VSCodium.
        set(STYR_WINDOWS_RUNTIME_DLLS
            "${STYR_UCRT64_PREFIX}/bin/SDL2.dll"
            "${STYR_UCRT64_PREFIX}/bin/glew32.dll"
            "${STYR_UCRT64_PREFIX}/bin/libstdc++-6.dll"
            "${STYR_UCRT64_PREFIX}/bin/libgcc_s_seh-1.dll"
            "${STYR_UCRT64_PREFIX}/bin/libwinpthread-1.dll"
            "${STYR_UCRT64_PREFIX}/bin/libiconv-2.dll"
        )
        foreach(STYR_RUNTIME_DLL IN LISTS STYR_WINDOWS_RUNTIME_DLLS)
            if(NOT EXISTS "${STYR_RUNTIME_DLL}")
                message(FATAL_ERROR
                    "Required UCRT64 runtime DLL was not found: ${STYR_RUNTIME_DLL}. "
                    "Install/update the Windows simulator prerequisites with: "
                    "C:/msys64/usr/bin/pacman.exe -S --needed "
                    "mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-SDL2 "
                    "mingw-w64-ucrt-x86_64-glew"
                )
            endif()
        endforeach()

        message(STATUS "SDL2 include: ${STYR_SDL2_INCLUDE_DIR}")
        message(STATUS "SDL2 library: ${STYR_SDL2_LIBRARY}")
        message(STATUS "GLEW include: ${STYR_GLEW_INCLUDE_DIR}")
        message(STATUS "GLEW library: ${STYR_GLEW_LIBRARY}")
        message(STATUS "Windows runtime DLLs will be staged beside host executables")
    else()
        find_package(SDL2 REQUIRED)
    endif()
endif()

add_subdirectory(${STYR_THIRD_PARTY_SIM_DIR}/rtmidi ${CMAKE_CURRENT_BINARY_DIR}/third_party/rtmidi)

set(STYR_SHARED_CORE_SOURCES
    ${STYR_THIRD_PARTY_COMMON_DIR}/stb/stb_sprintf.c
    ${STYR_THIRD_PARTY_COMMON_DIR}/ff/ff.c
    ${STYR_SHARED_DIR}/core/Debug.cpp
    ${STYR_SHARED_DIR}/core/fs/Error.cpp
    ${STYR_SHARED_DIR}/core/fs/File.cpp
    ${STYR_SHARED_DIR}/core/fs/FileSystem.cpp
    ${STYR_SHARED_DIR}/core/fs/Volume.cpp
    ${STYR_SHARED_DIR}/core/gfx/Canvas.cpp
    ${STYR_SHARED_DIR}/core/math/Mat3.cpp
    ${STYR_SHARED_DIR}/core/math/Mat4.cpp
    ${STYR_SHARED_DIR}/core/math/Math.cpp
    ${STYR_SHARED_DIR}/core/math/Vec2.cpp
    ${STYR_SHARED_DIR}/core/math/Vec3.cpp
    ${STYR_SHARED_DIR}/core/math/Vec4.cpp
    ${STYR_SHARED_DIR}/core/midi/MidiMessage.cpp
    ${STYR_SHARED_DIR}/core/midi/MidiParser.cpp
    ${STYR_SHARED_DIR}/core/profiler/Profiler.cpp
)

set(STYR_SIM_PLATFORM_SOURCES
    ${STYR_SOLOUD_DIR}/src/audiosource/wav/dr_impl.cpp
    ${STYR_SOLOUD_DIR}/src/audiosource/wav/soloud_wav.cpp
    ${STYR_SOLOUD_DIR}/src/audiosource/wav/stb_vorbis.c
    ${STYR_SOLOUD_DIR}/src/backend/sdl2_static/soloud_sdl2_static.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_audiosource.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_bus.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_3d.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_basicops.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_faderops.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_filterops.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_getters.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_setters.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_voicegroup.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_core_voiceops.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_fader.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_fft_lut.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_fft.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_file.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_filter.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud_thread.cpp
    ${STYR_SOLOUD_DIR}/src/core/soloud.cpp
    ${STYR_THIRD_PARTY_SIM_DIR}/stb/stb_image_write.cpp
    ${STYR_NANOVG_DIR}/src/nanovg.c
    ${STYR_SIM_PLATFORM_DIR}/drivers/Console.cpp
    core/Simulator.cpp
    core/TargetStateTracker.cpp
    core/TargetTrace.cpp
    core/TargetTracePlayer.cpp
    core/TargetTraceRecorder.cpp
    core/frontend/Audio.cpp
    core/frontend/Frontend.cpp
    core/frontend/InstrumentSetup.cpp
    core/frontend/Midi.cpp
    core/frontend/Renderer.cpp
    core/frontend/Window.cpp
    core/frontend/instruments/DrumSampler.cpp
    core/frontend/instruments/Synth.cpp
    ${STYR_SIM_PLATFORM_DIR}/os/os.cpp
)

# The shared low-level core still calls into the platform layer and vice versa,
# so they remain one host runtime target until that historical coupling is
# reduced.  The repository boundary is nevertheless explicit and stable.
add_library(styr_runtime STATIC ${STYR_SHARED_CORE_SOURCES} ${STYR_SIM_PLATFORM_SOURCES})
target_include_directories(styr_runtime PUBLIC
    ${STYR_SEQUENCER_DIR}
    ${STYR_SHARED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STYR_SIM_PLATFORM_DIR}
    ${STYR_THIRD_PARTY_COMMON_DIR}
    ${STYR_THIRD_PARTY_SIM_DIR}
    ${STYR_ARGS_DIR}
    ${STYR_NANOVG_DIR}/src
    ${STYR_THIRD_PARTY_SIM_DIR}/rtmidi
    ${STYR_SOLOUD_DIR}/include
    ${STYR_TINYFORMAT_DIR}
)
target_compile_definitions(styr_runtime PUBLIC PLATFORM_SIM WITH_SDL2_STATIC)
if(WIN32)
    # The simulator and test executables provide normal C/C++ main() entry
    # points. Do not let SDL headers rename them to SDL_main().
    target_compile_definitions(styr_runtime PUBLIC SDL_MAIN_HANDLED)
endif()
target_compile_options(styr_runtime PRIVATE -Wall -Wno-unused-function -Wno-unknown-pragmas -Wno-unknown-attributes -fPIC -fdata-sections -ffunction-sections)
target_link_libraries(styr_runtime PUBLIC rtmidi)

if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    separate_arguments(STYR_EMSCRIPTEN_FLAGS_LIST UNIX_COMMAND "${STYR_EMSCRIPTEN_FLAGS}")
    target_compile_options(styr_runtime PUBLIC ${STYR_EMSCRIPTEN_FLAGS_LIST})
    target_link_options(styr_runtime PUBLIC ${STYR_EMSCRIPTEN_FLAGS_LIST})
else()
    if(WIN32)
        # Link only the SDL2 runtime import library here. SDL2main and
        # -mwindows are application-entry-point concerns and must not propagate
        # through styr_runtime into unit/integration test executables.
        target_include_directories(styr_runtime PUBLIC
            ${STYR_SDL2_INCLUDE_DIR}
            ${STYR_SDL2_INCLUDE_DIR}/..
        )
        target_link_libraries(styr_runtime PUBLIC
            styr_sdl2
            ${OPENGL_LIBRARIES}
            styr_glew
        )
    else()
        target_include_directories(styr_runtime PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_INCLUDE_DIR}/..)
        target_link_libraries(styr_runtime PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
    endif()
endif()

string(LENGTH "${STYR_ROOT}/" SOURCE_PATH_SIZE)
target_compile_definitions(styr_runtime PUBLIC SOURCE_PATH_SIZE=${SOURCE_PATH_SIZE})

add_subdirectory(${STYR_SEQUENCER_DIR} ${CMAKE_CURRENT_BINARY_DIR}/sequencer)

function(platform_postprocess_executable executable)
    if(WIN32)
        # Keep native host executables independent of the caller's PATH.  The
        # application directory is part of the Windows DLL search order, so
        # staging the exact UCRT64 DLL set here prevents accidental mixing with
        # another MinGW/libstdc++ installation.
        add_custom_command(TARGET ${executable} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${STYR_WINDOWS_RUNTIME_DLLS}
                $<TARGET_FILE_DIR:${executable}>
            COMMENT "Staging MSYS2 UCRT64 runtime DLLs for ${executable}"
            VERBATIM
        )
    endif()
endfunction()

add_executable(styr_simulator main.cpp)
target_link_libraries(styr_simulator PRIVATE styr_sequencer)
platform_postprocess_executable(styr_simulator)

# Headless documentation capture tool. It drives the same SequencerApp/Simulator
# pair as the desktop simulator but never creates a window. Screenshots are
# written directly from the LCD framebuffer, making the result deterministic
# and cross-platform.
if(NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    add_executable(styr_manual_screenshots EXCLUDE_FROM_ALL tools/manual_screenshots.cpp)
    target_link_libraries(styr_manual_screenshots PRIVATE styr_sequencer)
    platform_postprocess_executable(styr_manual_screenshots)

    set(STYR_MANUAL_SCREENSHOT_STAGING "${CMAKE_CURRENT_BINARY_DIR}/manual-screenshots-staging")
    add_custom_target(manual-screenshots
        # Generate into the build tree first. Only publish to docs/manual/assets
        # after the complete capture run succeeds, so a failed run can never
        # leave the checked-out manual with a half-updated screenshot set.
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${STYR_MANUAL_SCREENSHOT_STAGING}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STYR_MANUAL_SCREENSHOT_STAGING}"
        # Run each documentation group in a fresh process. This prevents UI,
        # observer and model state from leaking between unrelated capture
        # groups and makes a failure immediately attributable to one section.
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" global
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" note
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" curve
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" midi-cv
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" lfo
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" generators
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" song
        COMMAND $<TARGET_FILE:styr_manual_screenshots> "${STYR_MANUAL_SCREENSHOT_STAGING}" "${STYR_DOC_SCREENSHOT_SCALE}" system
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STYR_ROOT}/docs/manual/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${STYR_MANUAL_SCREENSHOT_STAGING}"
            "${STYR_ROOT}/docs/manual/assets"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${STYR_MANUAL_SCREENSHOT_STAGING}"
        DEPENDS styr_manual_screenshots
        WORKING_DIRECTORY "${STYR_ROOT}"
        COMMENT "Generating and publishing Styr manual LCD screenshots (${STYR_DOC_SCREENSHOT_SCALE}x nearest-neighbour)"
        VERBATIM
    )
endif()
if(WIN32)
    # Native Windows builds do not require Developer Mode or elevated
    # symlink privileges just to make simulator assets available.
    add_custom_command(TARGET styr_simulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
    )
else()
    add_custom_command(TARGET styr_simulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${CMAKE_CURRENT_BINARY_DIR}/assets
    )
endif()


if(STYR_BUILD_TESTS AND NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    enable_testing()
    add_subdirectory(${STYR_ROOT}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif()

if(STYR_SIM_ENABLE_PYTHON AND NOT CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    add_subdirectory(${STYR_PYBIND11_DIR} ${CMAKE_CURRENT_BINARY_DIR}/third_party/pybind11)
    add_subdirectory(python)
endif()
