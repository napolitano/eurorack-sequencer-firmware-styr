# Purpose: Provides Python automation for the PlatformIO build integration.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Styr PlatformIO build glue.

The repository is organized for developers, not for a build system:
``src`` contains Styr code and ``third_party`` contains imported code.  This
script only maps that tree into PlatformIO's SCons graph.  It deliberately
contains no toolchain installation or dependency discovery logic.
"""

from pathlib import Path

Import("env")  # type: ignore[name-defined]

ROOT = Path(env.subst("$PROJECT_DIR")).resolve()
BUILD = Path(env.subst("$BUILD_DIR"))
SHARED = ROOT / "src" / "shared"
SEQUENCER = ROOT / "src" / "sequencer"
STM32 = SEQUENCER / "platform" / "stm32"
THIRD_COMMON = ROOT / "third_party" / "common"
THIRD_EMBEDDED = ROOT / "third_party" / "embedded"
FREERTOS = THIRD_EMBEDDED / "freertos"
LIBUSBHOST = THIRD_EMBEDDED / "libusbhost"
BOOTLOADER_THIRD_PARTY = THIRD_EMBEDDED / "bootloader"

MACHINE_LINK_FLAGS = [
    "-mcpu=cortex-m4",
    "-mthumb",
    "-mfloat-abi=hard",
    "-mfpu=fpv4-sp-d16",
]

# Generic cross-target core facilities. They remain part of the host/runtime
# graph because parts of the historical core still depend on the platform API.
SHARED_CORE_FILTER = [
    "+<core/Debug.cpp>",
    "+<core/fs/Error.cpp>",
    "+<core/fs/File.cpp>",
    "+<core/fs/FileSystem.cpp>",
    "+<core/fs/Volume.cpp>",
    "+<core/gfx/Canvas.cpp>",
    "+<core/math/Mat3.cpp>",
    "+<core/math/Mat4.cpp>",
    "+<core/math/Math.cpp>",
    "+<core/math/Vec2.cpp>",
    "+<core/math/Vec3.cpp>",
    "+<core/math/Vec4.cpp>",
    "+<core/midi/MidiMessage.cpp>",
    "+<core/midi/MidiParser.cpp>",
    "+<core/profiler/Profiler.cpp>",
]

SEQUENCER_FILTER = [
    "+<asteroids/**>",
    "+<engine/**>",
    "+<intro/**>",
    "+<model/**>",
    "+<ui/**>",
]

STM32_RUNTIME_FILTER = [
    "+<os/board.cpp>",
    "+<os/os.cpp>",
    "+<drivers/Adc.cpp>",
    "+<drivers/ButtonLedMatrix.cpp>",
    "+<drivers/ClockTimer.cpp>",
    "+<drivers/Console.cpp>",
    "+<drivers/Dac.cpp>",
    "+<drivers/Dio.cpp>",
    "+<drivers/Encoder.cpp>",
    "+<drivers/GateOutput.cpp>",
    "+<drivers/HighResolutionTimer.cpp>",
    "+<drivers/Lcd.cpp>",
    "+<drivers/Midi.cpp>",
    "+<drivers/SdCard.cpp>",
    "+<drivers/ShiftRegister.cpp>",
    "+<drivers/System.cpp>",
    "+<drivers/UsbH.cpp>",
]

FREERTOS_FILTER = [
    "+<kernel/croutine.c>",
    "+<kernel/event_groups.c>",
    "+<kernel/list.c>",
    "+<kernel/queue.c>",
    "+<kernel/tasks.c>",
    "+<kernel/timers.c>",
    "+<portable/port.c>",
]

LIBUSBHOST_FILTER = [
    "+<src/usbh_core.c>",
    "+<src/usbh_driver_ac_midi.c>",
    "+<src/usbh_driver_hub.c>",
    "+<src/usbh_lld_stm32f4.c>",
]

BOOTLOADER_SUPPORT_FILTER = [
    "+<lib/ff/ff.c>",
    "+<md5/MD5.cpp>",
]


def option(name: str, default: str = "") -> str:
    value = env.GetProjectOption(name, default)
    return value.strip() if isinstance(value, str) else str(value).strip()


def source_path_size() -> int:
    # Preserve the historical SOURCE_PATH_SIZE contract used by debug output.
    return len(str(ROOT)) + 1


def build_external_sources() -> None:
    components = {
        item.strip()
        for item in option("custom_styr_components").split(",")
        if item.strip()
    }

    if "runtime" in components:
        env.BuildSources(
            str(BUILD / "external" / "shared_core"),
            str(SHARED),
            SHARED_CORE_FILTER,
        )
        env.BuildSources(
            str(BUILD / "external" / "third_party" / "fatfs"),
            str(THIRD_COMMON / "ff"),
            ["+<ff.c>"],
        )
        env.BuildSources(
            str(BUILD / "external" / "third_party" / "stb"),
            str(THIRD_COMMON / "stb"),
            ["+<stb_sprintf.c>"],
        )
        env.BuildSources(
            str(BUILD / "external" / "stm32"),
            str(STM32),
            STM32_RUNTIME_FILTER,
        )
        env.BuildSources(
            str(BUILD / "external" / "third_party" / "freertos"),
            str(FREERTOS),
            FREERTOS_FILTER,
        )
        env.BuildSources(
            str(BUILD / "external" / "third_party" / "libusbhost"),
            str(LIBUSBHOST),
            LIBUSBHOST_FILTER,
        )

    if "sequencer" in components:
        env.BuildSources(
            str(BUILD / "external" / "sequencer"),
            str(SEQUENCER),
            SEQUENCER_FILTER,
        )

    if "bootloader" in components:
        env.BuildSources(
            str(BUILD / "external" / "third_party" / "bootloader"),
            str(BOOTLOADER_THIRD_PARTY),
            BOOTLOADER_SUPPORT_FILTER,
        )


def map_file_linker_flag(artifact_name: str) -> str:
    """Return a Windows-safe linker map flag.

    $BUILD_DIR expands to an absolute path.  Passing that path through
    ``-Wl,-Map=...`` breaks when the project directory contains spaces because
    GCC's linker-option forwarding does not preserve PlatformIO/SCons quoting
    reliably on Windows.  Our build directory is deliberately inside the
    project, so pass the map path relative to the project root instead.
    """
    map_file = (BUILD / f"{artifact_name}.map").resolve()
    try:
        relative_map = map_file.relative_to(ROOT).as_posix()
    except ValueError as exc:
        raise RuntimeError(
            "PlatformIO build_dir must be inside the Styr project root so the "
            "linker map path remains portable on Windows"
        ) from exc
    return f"-Wl,-Map={relative_map}"


def configure_environment() -> None:
    artifact_name = option("custom_styr_artifact_name", "firmware")
    linker_script = (ROOT / option("custom_styr_ldscript")).resolve()
    if not linker_script.is_file():
        raise RuntimeError(f"Linker script not found: {linker_script}")

    env.Replace(
        PROGNAME=artifact_name,
        LDSCRIPT_PATH=str(linker_script),
        COMPILATIONDB_INCLUDE_TOOLCHAIN=True,
        COMPILATIONDB_PATH=str(ROOT / "compile_commands.json"),
        OBJDUMP="arm-none-eabi-objdump",
    )

    env.Append(CXXFLAGS=["-std=c++11", "-Wno-unused-function"])

    env.Append(
        CPPPATH=[
            str(SEQUENCER),
            str(SHARED),
            str(STM32),
            str(THIRD_COMMON),
            str(FREERTOS / "include"),
            str(FREERTOS / "portable"),
            str(LIBUSBHOST / "include"),
            str(ROOT / "src" / "hwconfig"),
            str(ROOT / "src" / "bootloader"),
            str(BOOTLOADER_THIRD_PARTY),
            str(BOOTLOADER_THIRD_PARTY / "md5"),
        ],
        CPPDEFINES=[("SOURCE_PATH_SIZE", source_path_size())],
        LINKFLAGS=[
            "-nostartfiles",
            "--specs=nano.specs",
            "--specs=nosys.specs",
            "-Wl,--gc-sections",
            map_file_linker_flag(artifact_name),
            *MACHINE_LINK_FLAGS,
        ],
    )


configure_environment()
build_external_sources()
