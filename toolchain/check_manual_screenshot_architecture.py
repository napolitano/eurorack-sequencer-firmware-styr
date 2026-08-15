#!/usr/bin/env python3
# Purpose: Enforces that manual screenshot generation remains independent of the interactive graphics frontend.
# Author: Axel Napolitano — Styr documentation tooling
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Validate the headless manual-screenshot build boundary."""

from __future__ import annotations

import argparse
from pathlib import Path
import platform
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SIM_CMAKE = ROOT / "src" / "simulator" / "CMakeLists.txt"
DEPS_CMAKE = ROOT / "src" / "simulator" / "cmake" / "StyrDependencies.cmake"
PRESETS = ROOT / "src" / "simulator" / "CMakePresets.json"
TESTS_CMAKE = ROOT / "src" / "simulator" / "tests" / "CMakeLists.txt"
INTEGRATION_CMAKE = ROOT / "src" / "simulator" / "tests" / "integration" / "CMakeLists.txt"

FORBIDDEN_SOURCE_TOKENS = (
    "core/frontend/",
    "STYR_SOLOUD_DIR",
    "STYR_NANOVG_DIR",
    "RtMidi",
)
FORBIDDEN_BINARY_PATTERNS = (
    r"SDL",
    r"GLEW",
    r"(?:^|[/\\])libGL(?:X|U)?[.]",
    r"opengl32[.]dll",
    r"(?:^|[/\\])libX11[.]",
    r"wayland",
)


def block(text: str, variable: str) -> str:
    match = re.search(rf"set\({re.escape(variable)}\s+(.*?)\n\)", text, flags=re.DOTALL)
    if not match:
        raise ValueError(f"missing CMake source block: {variable}")
    return match.group(1)


def static_errors() -> list[str]:
    errors: list[str] = []
    cmake = SIM_CMAKE.read_text(encoding="utf-8")
    deps = DEPS_CMAKE.read_text(encoding="utf-8")
    presets = PRESETS.read_text(encoding="utf-8")
    tests_cmake = TESTS_CMAKE.read_text(encoding="utf-8")
    integration_cmake = INTEGRATION_CMAKE.read_text(encoding="utf-8")

    for token in (
        'option(STYR_SIM_BUILD_FRONTEND',
        'add_library(styr_runtime STATIC',
        'add_library(styr_frontend STATIC',
        'target_link_libraries(styr_manual_screenshots PRIVATE styr_sequencer)',
    ):
        if token not in cmake:
            errors.append(f"simulator CMake missing headless architecture token: {token}")

    try:
        headless_sources = block(cmake, "STYR_SIM_HEADLESS_SOURCES")
    except ValueError as exc:
        errors.append(str(exc))
    else:
        for token in FORBIDDEN_SOURCE_TOKENS:
            if token in headless_sources:
                errors.append(f"headless runtime contains frontend-only source/dependency token: {token}")

    if re.search(r"target_link_libraries\(styr_runtime\b[^)]*(?:SDL|OpenGL|GLEW|rtmidi|styr_frontend)", cmake, re.DOTALL | re.IGNORECASE):
        errors.append("styr_runtime links a frontend/window-system dependency")

    if "if(STYR_SIM_BUILD_FRONTEND)" not in deps:
        errors.append("simulator dependency resolver does not guard frontend-only dependencies")

    for token in (
        '"name": "manual-screenshots"',
        '"name": "windows-ucrt64-manual-screenshots"',
        '"STYR_SIM_BUILD_FRONTEND": "OFF"',
        '"STYR_SIM_FETCH_DEPENDENCIES": "OFF"',
    ):
        if token not in presets:
            errors.append(f"headless screenshot preset contract missing token: {token}")

    # Interactive integration tests include IntegrationTestRunner -> Frontend.
    # They must remain on the frontend side of the boundary and must disappear
    # entirely when a headless configuration disables the frontend.
    if not re.search(
        r"if\(STYR_SIM_BUILD_FRONTEND\).*?add_subdirectory\(integration\).*?endif\(\)",
        tests_cmake,
        flags=re.DOTALL,
    ):
        errors.append("interactive integration tests are not guarded by STYR_SIM_BUILD_FRONTEND")
    if not re.search(
        r"target_link_libraries\(\$\{test\}\s+PRIVATE\s+styr_frontend\s*\)",
        integration_cmake,
        flags=re.DOTALL,
    ):
        errors.append("interactive integration tests do not link the styr_frontend target")
    if "platform_postprocess_frontend_executable(${test})" not in integration_cmake:
        errors.append("interactive integration tests do not stage frontend runtime dependencies")

    return errors


def dependency_output(binary: Path) -> str:
    system = platform.system()
    if system == "Linux":
        command = ["ldd", str(binary)]
    elif system == "Darwin":
        command = ["otool", "-L", str(binary)]
    elif system == "Windows":
        command = ["objdump", "-p", str(binary)]
    else:
        raise RuntimeError(f"unsupported platform for binary dependency inspection: {system}")
    return subprocess.run(command, check=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout


def binary_errors(binary: Path) -> list[str]:
    if not binary.is_file():
        return [f"manual screenshot binary does not exist: {binary}"]
    try:
        output = dependency_output(binary)
    except (OSError, subprocess.CalledProcessError, RuntimeError) as exc:
        return [f"could not inspect manual screenshot binary dependencies: {exc}"]

    errors: list[str] = []
    for pattern in FORBIDDEN_BINARY_PATTERNS:
        if re.search(pattern, output, flags=re.IGNORECASE | re.MULTILINE):
            errors.append(f"manual screenshot binary has forbidden graphics/window dependency matching: {pattern}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path, help="Optional built styr_manual_screenshots executable to inspect")
    args = parser.parse_args()

    errors = static_errors()
    if args.binary is not None:
        errors.extend(binary_errors(args.binary.resolve()))

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"Manual screenshot architecture FAILED ({len(errors)} error(s)).", file=sys.stderr)
        return 1

    print("Manual screenshot architecture OK: framebuffer capture is isolated from the interactive graphics frontend.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
