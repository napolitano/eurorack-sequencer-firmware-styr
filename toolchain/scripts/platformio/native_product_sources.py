# Purpose: Builds Styr production code for PlatformIO native unit tests.
# Author: Axel Napolitano — Styr build-system modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Native PlatformIO test build adapter.

PlatformIO owns all tests for code that is also built by PlatformIO. Native
unit tests reuse the simulator platform headers only as deterministic host-side
I/O fakes; CMake/CTest remains reserved for simulator-specific behavior.

On Windows the Native platform depends on a host GCC installation. Styr uses
MSYS2 UCRT64 for its other native Windows builds, so this adapter resolves that
same toolchain explicitly and exports its runtime directories to SCons. This
avoids an opaque ``Error 1`` when PlatformIO can name ``g++`` but the compiler
or one of its helper/runtime executables is not actually launchable.
"""

from pathlib import Path
import os
import shutil
import subprocess

Import("env")  # type: ignore[name-defined]

ROOT = Path(env.subst("$PROJECT_DIR")).resolve()
BUILD = Path(env.subst("$BUILD_DIR"))
SHARED = ROOT / "src" / "shared"
SEQUENCER = ROOT / "src" / "sequencer"
SIMULATOR = ROOT / "src" / "simulator"
SIM_PLATFORM = SIMULATOR / "platform"
THIRD_COMMON = ROOT / "third_party" / "common"
THIRD_SIM = ROOT / "third_party" / "simulator"
THIRD_SIM_STB = THIRD_SIM / "stb"
BOOTLOADER = ROOT / "src" / "bootloader"
BOOTLOADER_THIRD_PARTY = ROOT / "third_party" / "embedded" / "bootloader"
PIO_TEST_ROOT = ROOT / "test"

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

SIM_TEST_SUPPORT_FILTER = [
    "+<core/Simulator.cpp>",
    "+<core/TargetStateTracker.cpp>",
    "+<platform/drivers/Console.cpp>",
    "+<platform/os/os.cpp>",
]

BOOTLOADER_TEST_FILTER = [
    "+<Format.cpp>",
    "+<UpdateFile.cpp>",
]


def option(name: str, default: str = "") -> str:
    value = env.GetProjectOption(name, default)
    return value.strip() if isinstance(value, str) else str(value).strip()


def source_path_size() -> int:
    return len(str(ROOT)) + 1


def _windows_toolchain_candidates():
    explicit = os.environ.get("STYR_NATIVE_TOOLCHAIN_BIN", "").strip()
    if explicit:
        yield Path(explicit)

    msys_root = Path(os.environ.get("STYR_MSYS2_ROOT", "C:/msys64"))
    # UCRT64 is Styr's canonical Windows host compiler. MINGW64 is accepted as
    # a last-resort compatibility fallback for developers with an older setup.
    yield msys_root / "ucrt64" / "bin"
    yield msys_root / "mingw64" / "bin"

    found = shutil.which("g++.exe") or shutil.which("g++")
    if found:
        yield Path(found).resolve().parent


def configure_native_toolchain() -> None:
    if os.name != "nt":
        return

    tool_dir = None
    for candidate in _windows_toolchain_candidates():
        if (candidate / "gcc.exe").is_file() and (candidate / "g++.exe").is_file():
            tool_dir = candidate.resolve()
            break

    if tool_dir is None:
        raise RuntimeError(
            "Styr native PlatformIO tests require a Windows GCC toolchain. "
            "Install MSYS2 UCRT64 (mingw-w64-ucrt-x86_64-gcc), or set "
            "STYR_NATIVE_TOOLCHAIN_BIN to the directory containing gcc.exe "
            "and g++.exe."
        )

    msys_root = tool_dir.parent.parent
    usr_bin = msys_root / "usr" / "bin"

    # PlatformIO's Native platform uses the system toolchain. Resolve the
    # executable names here rather than relying on SCons' inherited PATH.
    tools = {
        "CC": tool_dir / "gcc.exe",
        "CXX": tool_dir / "g++.exe",
        "LINK": tool_dir / "g++.exe",
        "AR": tool_dir / "ar.exe",
        "RANLIB": tool_dir / "ranlib.exe",
        "OBJDUMP": tool_dir / "objdump.exe",
    }
    missing = [str(path) for path in tools.values() if not path.is_file()]
    if missing:
        raise RuntimeError("Incomplete Styr native GCC toolchain: " + ", ".join(missing))

    env.PrependENVPath("PATH", str(tool_dir))
    if usr_bin.is_dir():
        env.PrependENVPath("PATH", str(usr_bin))
    env.Replace(**{name: str(path) for name, path in tools.items()})

    # Fail early with a useful diagnostic instead of letting every Unity suite
    # die independently with a bare SCons "Error 1".
    probe_env = dict(os.environ)
    probe_env.update({str(k): str(v) for k, v in env.get("ENV", {}).items()})
    probe = subprocess.run(
        [str(tools["CXX"]), "--version"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=probe_env,
        check=False,
    )
    if probe.returncode != 0:
        output = (probe.stdout or "").strip()
        raise RuntimeError(
            "MSYS2 UCRT64 g++ was found but cannot be executed by PlatformIO. "
            f"Compiler: {tools['CXX']}. Output: {output or '<none>'}"
        )

    first_line = (probe.stdout or "g++").splitlines()[0]
    print(f"Styr native toolchain: {tools['CXX']} ({first_line})")

    # PlatformIO launches each Unity executable as a normal unpackaged Windows
    # process. Do not rely on the developer's global PATH for MinGW runtime
    # resolution: stage the DLL closure from the exact GCC toolchain selected
    # above next to the executable. Windows searches the application directory
    # before PATH directories for ordinary unpackaged applications, so the test
    # binary and its runtime stay version-matched without forcing a fully static
    # CRT/libstdc++ link.
    def imported_dlls(binary: Path):
        inspect_env = dict(os.environ)
        inspect_env.update({str(k): str(v) for k, v in env.get("ENV", {}).items()})
        result = subprocess.run(
            [str(tools["OBJDUMP"]), "-p", str(binary)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=inspect_env,
            check=False,
        )
        if result.returncode != 0:
            output = (result.stdout or "").strip()
            raise RuntimeError(
                f"Could not inspect Windows runtime imports for {binary.name}: "
                f"{output or '<none>'}"
            )

        imports = []
        for line in (result.stdout or "").splitlines():
            line = line.strip()
            if line.lower().startswith("dll name:"):
                imports.append(line.split(":", 1)[1].strip())
        return imports

    toolchain_dlls = {
        path.name.lower(): path
        for path in tool_dir.glob("*.dll")
        if path.is_file()
    }

    def stage_toolchain_runtime(binary: Path):
        destination = binary.parent
        queue = [binary]
        inspected = set()
        staged = {}

        while queue:
            current = queue.pop(0)
            key = str(current).lower()
            if key in inspected:
                continue
            inspected.add(key)

            for dll_name in imported_dlls(current):
                source_dll = toolchain_dlls.get(dll_name.lower())
                if source_dll is None:
                    # System DLL: leave normal Windows resolution untouched.
                    continue

                destination_dll = destination / source_dll.name
                shutil.copy2(source_dll, destination_dll)
                staged[source_dll.name.lower()] = source_dll.name
                queue.append(source_dll)

        return [staged[name] for name in sorted(staged)]

    # Probe the exact dynamic runtime arrangement before building every suite.
    # A vector deliberately pulls in libstdc++; staging the transitive toolchain
    # DLL closure beside the probe catches the same loader class that previously
    # surfaced as STATUS_ENTRYPOINT_NOT_FOUND.
    probe_source = BUILD / "styr_native_runtime_probe.cpp"
    probe_exe = BUILD / "styr_native_runtime_probe.exe"
    probe_stamp = BUILD / "styr_native_runtime_probe.ok"
    probe_signature = f"{tools['CXX']}\n{first_line}\nlocal-runtime-bundle-v1\n"

    if not probe_stamp.is_file() or probe_stamp.read_text(encoding="utf-8", errors="replace") != probe_signature:
        BUILD.mkdir(parents=True, exist_ok=True)
        probe_source.write_text(
            "#include <vector>\n"
            "int main() { std::vector<int> values{1, 2, 3}; return values.size() == 3 ? 0 : 1; }\n",
            encoding="utf-8",
        )
        compile_probe = subprocess.run(
            [str(tools["CXX"]), "-std=c++11", str(probe_source), "-o", str(probe_exe)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=probe_env,
            check=False,
        )
        if compile_probe.returncode != 0:
            output = (compile_probe.stdout or "").strip()
            raise RuntimeError(
                "Styr could not link the Windows native C++ runtime probe. "
                f"Compiler: {tools['CXX']}. Output: {output or '<none>'}"
            )

        staged_probe_dlls = stage_toolchain_runtime(probe_exe)
        run_probe = subprocess.run(
            [str(probe_exe)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=probe_env,
            check=False,
        )
        if run_probe.returncode != 0:
            output = (run_probe.stdout or "").strip()
            raise RuntimeError(
                "Styr's Windows native runtime probe could not start with its local "
                "toolchain DLL bundle. "
                f"Exit code: {run_probe.returncode}. "
                f"Staged DLLs: {', '.join(staged_probe_dlls) or '<none>'}. "
                f"Output: {output or '<none>'}"
            )
        probe_stamp.write_text(probe_signature, encoding="utf-8")

    print("Styr native runtime: local UCRT64 toolchain DLL probe PASS")

    def stage_native_runtime(source, target, action_env):
        program = Path(target[0].get_abspath())
        staged = stage_toolchain_runtime(program)
        if not staged:
            print(f"Styr native runtime bundle: PASS ({program.name}; no toolchain DLL imports)")
            return
        print(
            f"Styr native runtime bundle: PASS ({program.name}; staged "
            + ", ".join(staged)
            + ")"
        )

    env.AddPostAction("$PROGPATH", stage_native_runtime)


def configure_common() -> None:
    configure_native_toolchain()
    env.Append(
        CPPPATH=[
            str(PIO_TEST_ROOT),
            str(PIO_TEST_ROOT / "core" / "io"),
            str(SHARED),
            str(SEQUENCER),
            str(SIMULATOR),
            str(SIM_PLATFORM),
            str(THIRD_COMMON),
            str(THIRD_SIM),
            str(BOOTLOADER),
            str(BOOTLOADER_THIRD_PARTY),
            str(BOOTLOADER_THIRD_PARTY / "md5"),
        ],
        CPPDEFINES=[
            "PLATFORM_SIM",
            ("SOURCE_PATH_SIZE", source_path_size()),
        ],
        # Warning/section flags come from platformio.ini so PlatformIO applies
        # them uniformly to product support, Unity and the suite itself. Keep
        # only the C++ language-level requirement here to avoid duplicate flags.
        CXXFLAGS=["-std=c++11"],
        LINKFLAGS=["-Wl,--gc-sections"],
    )


def build_bootloader_support() -> None:
    # Build bootloader production support as static libraries rather than
    # injecting every object directly into every Unity executable.  PlatformIO
    # runs each test_* directory as an independent program; archive semantics
    # let the linker extract only the objects actually referenced by that
    # suite.  For example, test_md5 must not acquire Format/UpdateFile and
    # therefore must not need Console or FatFs stubs.
    bootloader_library = env.BuildLibrary(
        str(BUILD / "native_test" / "bootloader_support"),
        str(BOOTLOADER),
        BOOTLOADER_TEST_FILTER,
    )
    md5_library = env.BuildLibrary(
        str(BUILD / "native_test" / "bootloader_md5_support"),
        str(BOOTLOADER_THIRD_PARTY / "md5"),
        ["+<MD5.cpp>"],
    )
    env.Append(LIBS=[bootloader_library, md5_library])


def build_product_support() -> None:
    # Keep the complete production graph available, but expose it to each
    # Unity suite through archives.  This mirrors normal library linkage and
    # prevents unrelated firmware objects from imposing their platform
    # dependencies on small focused tests.  PlatformIO groups LIBS for GCC, so
    # the historical cyclic dependencies between shared/sequencer host support
    # remain resolvable while unused objects stay out of the executable.
    libraries = [
        env.BuildLibrary(
            str(BUILD / "native_test" / "shared_core"),
            str(SHARED),
            SHARED_CORE_FILTER,
        ),
        env.BuildLibrary(
            str(BUILD / "native_test" / "third_party" / "fatfs"),
            str(THIRD_COMMON / "ff"),
            ["+<ff.c>"],
        ),
        env.BuildLibrary(
            str(BUILD / "native_test" / "third_party" / "stb"),
            str(THIRD_COMMON / "stb"),
            ["+<stb_sprintf.c>"],
        ),
        env.BuildLibrary(
            str(BUILD / "native_test" / "sequencer"),
            str(SEQUENCER),
            SEQUENCER_FILTER,
        ),
        env.BuildLibrary(
            str(BUILD / "native_test" / "simulator_support"),
            str(SIMULATOR),
            SIM_TEST_SUPPORT_FILTER,
        ),
        # Simulator.cpp contains the screenshot API and therefore references
        # stbi_write_png even when a particular Unity suite never writes an
        # image. Keep the implementation in its own archive so it is extracted
        # only when Simulator.o is pulled into the test executable.
        env.BuildLibrary(
            str(BUILD / "native_test" / "third_party" / "stb_image_write"),
            str(THIRD_SIM_STB),
            ["+<stb_image_write.cpp>"],
        ),
    ]
    env.Append(LIBS=libraries)


configure_common()
components = {
    item.strip()
    for item in option("custom_styr_test_components").split(",")
    if item.strip()
}

if "bootloader" in components:
    build_bootloader_support()
if "product" in components:
    build_product_support()
