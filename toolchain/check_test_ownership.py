#!/usr/bin/env python3
# Purpose: Enforces build-system and test-framework ownership for Styr tests.
# Author: Axel Napolitano — Styr build-system modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Enforce Styr's PlatformIO/Unity versus CMake simulator-test boundary."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
PIO_INI = (ROOT / "platformio.ini").read_text(encoding="utf-8")
CI = (ROOT / ".github" / "workflows" / "ci.yml").read_text(encoding="utf-8")
SIM_TEST_ROOT = ROOT / "src" / "simulator" / "tests"
SIM_CMAKE = (SIM_TEST_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")

NATIVE_ADAPTER = (
    ROOT / "toolchain" / "scripts" / "platformio" / "native_product_sources.py"
).read_text(encoding="utf-8")
STB_IMAGE_WRITE_CPP = ROOT / "third_party" / "simulator" / "stb" / "stb_image_write.cpp"

errors = []


def require(condition: bool, message: str) -> None:
    if not condition:
        errors.append(message)


def ini_section(name: str) -> str:
    match = re.search(
        rf"(?ms)^\[{re.escape(name)}\]\s*$\n(.*?)(?=^\[|\Z)",
        PIO_INI,
    )
    return match.group(1) if match else ""


require(re.search(r"(?m)^test_dir\s*=\s*test\s*$", PIO_INI) is not None,
        "platformio.ini must set test_dir = test")
require("[embedded]" in PIO_INI,
        "embedded common configuration must live in [embedded], not global [env]")
require(re.search(r"(?m)^\[env\]\s*$", PIO_INI) is None,
        "global [env] would leak STM32 settings into native PlatformIO tests")
require("[env:test_bootloader_native]" in PIO_INI,
        "missing PlatformIO bootloader native test environment")
require("[env:test_product_native]" in PIO_INI,
        "missing PlatformIO product native test environment")
for native_env in ("env:test_bootloader_native", "env:test_product_native"):
    body = ini_section(native_env)
    require(re.search(r"(?m)^test_framework\s*=\s*unity\s*$", body) is not None,
            f"{native_env} must use PlatformIO Unity")
    ldf = re.search(r"(?m)^lib_ldf_mode\s*=\s*([^;#\s]+)", body)
    require(ldf is not None,
            f"{native_env} must declare lib_ldf_mode explicitly")
    if ldf is not None:
        require(ldf.group(1) in {"chain", "chain+", "deep", "deep+"},
                f"{native_env} must keep LDF enabled so PlatformIO can resolve <unity.h>; "
                f"got lib_ldf_mode = {ldf.group(1)}")

product_env = ini_section("env:test_product_native")
require(re.search(r"(?m)^\s*bootloader/\*\s*$", product_env) is not None,
        "test_product_native must include bootloader/*; Product includes the bootloader")
require(re.search(r"(?m)^\s*core/\*\s*$", product_env) is not None,
        "test_product_native must include core/*")
require(re.search(r"(?m)^\s*sequencer/\*\s*$", product_env) is not None,
        "test_product_native must include sequencer/*")
require(re.search(r"(?m)^custom_styr_test_components\s*=\s*[^\n]*\bproduct\b[^\n]*\bbootloader\b", product_env) is not None,
        "test_product_native must build both product and bootloader support")

require("test_framework = custom" not in PIO_INI,
        "custom PlatformIO test frameworks are not permitted for product tests")
require(not (ROOT / "test" / "test_custom_runner.py").exists(),
        "legacy PlatformIO custom runner must not exist")
require((ROOT / "toolchain" / "scripts" / "platformio" / "native_product_sources.py").is_file(),
        "missing PlatformIO native product-source build adapter")
require("def build_bootloader_support" in NATIVE_ADAPTER,
        "native adapter must define bootloader support assembly")
require(STB_IMAGE_WRITE_CPP.is_file(),
        "missing simulator stb_image_write implementation source")
if STB_IMAGE_WRITE_CPP.is_file():
    require("STB_IMAGE_WRITE_IMPLEMENTATION" in STB_IMAGE_WRITE_CPP.read_text(encoding="utf-8", errors="replace"),
            "simulator stb_image_write source must instantiate STB_IMAGE_WRITE_IMPLEMENTATION")
require("styr_native_runtime_probe" in NATIVE_ADAPTER,
        "Windows native PlatformIO tests must preflight the C++ runtime")
require("stage_toolchain_runtime" in NATIVE_ADAPTER,
        "Windows native PlatformIO tests must stage the selected toolchain DLL closure locally")
require("imported_dlls" in NATIVE_ADAPTER and "OBJDUMP" in NATIVE_ADAPTER,
        "Windows native runtime staging must derive imports from the linked PE binaries")
require("shutil.copy2" in NATIVE_ADAPTER,
        "Windows native runtime staging must copy matching toolchain DLLs beside program.exe")
require(re.search(r"def\s+stage_native_runtime\(\s*target\s*,\s*source\s*,\s*env\s*\)\s*:", NATIVE_ADAPTER) is not None,
        "SCons post actions must use the canonical (target, source, env) callback signature")
require('env.Append(LINKFLAGS=["-static"])' not in NATIVE_ADAPTER,
        "native tests must not force a fully static Windows runtime link")
bootloader_support = re.search(
    r"(?ms)^def build_bootloader_support\(\).*?(?=^def |\Z)",
    NATIVE_ADAPTER,
)
require(bootloader_support is not None,
        "cannot inspect bootloader support assembly")
if bootloader_support is not None:
    body = bootloader_support.group(0)
    require("BuildLibrary(" in body,
            "bootloader native support must use static libraries for per-suite link isolation")
    require("BuildSources(" not in body,
            "bootloader native support must not inject every production object into every Unity suite")
    require("env.Append(LIBS=" in body,
            "bootloader native support libraries must be attached to the link graph")
product_support = re.search(
    r"(?ms)^def build_product_support\(\).*?(?=^def |^configure_common\(\)|\Z)",
    NATIVE_ADAPTER,
)
require(product_support is not None,
        "cannot inspect product support assembly")
if product_support is not None:
    body = product_support.group(0)
    require("BuildLibrary(" in body,
            "native product support must use static libraries for per-suite link isolation")
    require("BuildSources(" not in body,
            "native product support must not inject the whole firmware graph into every Unity suite")
    require("env.Append(LIBS=" in body,
            "native product support libraries must be attached to the link graph")
    require("stb_image_write.cpp" in body,
            "native simulator support must link the stb_image_write implementation required by Simulator.cpp")
require("pio test -e test_bootloader_native" in CI,
        "CI must run bootloader tests through PlatformIO")
require("PIOTEST_RUNNING_NAME" in NATIVE_ADAPTER,
        "complete product tests must isolate bootloader versus core/sequencer support per PlatformIO suite")
require('running_suite.startswith("bootloader/")' in NATIVE_ADAPTER,
        "test_product_native must select bootloader support only for bootloader suites")
require("pio test -e test_product_native" in CI,
        "CI must run the complete native product suite through PlatformIO")

product_tests = sorted((ROOT / "test").rglob("*.cpp"))
bootloader_tests = sorted((ROOT / "test" / "bootloader").rglob("*.cpp"))
core_tests = sorted((ROOT / "test" / "core").rglob("*.cpp"))
sequencer_tests = sorted((ROOT / "test" / "sequencer").rglob("*.cpp"))
require(len(product_tests) == 44,
        f"expected 44 PlatformIO product test suites, found {len(product_tests)}")
require(len(bootloader_tests) == 5,
        f"expected 5 bootloader PlatformIO tests, found {len(bootloader_tests)}")
require(len(core_tests) == 6,
        f"expected 6 shared-core PlatformIO tests, found {len(core_tests)}")
require(len(sequencer_tests) == 33,
        f"expected 33 sequencer PlatformIO tests, found {len(sequencer_tests)}")

legacy_tokens = (
    '#include "UnitTest.h"',
    "UNIT_TEST(",
    "CASE(\"",
    "expectEqual(",
    "expectTrue(",
    "expectFalse(",
)

unity_case_count = 0
for path in product_tests:
    relative = path.relative_to(ROOT / "test")
    suite_dirs = [part for part in relative.parts[:-1] if part.startswith("test_")]
    require(bool(suite_dirs), f"PlatformIO test lacks a test_* suite directory: {relative}")
    source = path.read_text(encoding="utf-8", errors="replace")
    require("#include <unity.h>" in source or '#include "unity.h"' in source,
            f"PlatformIO product test must include Unity directly: {relative}")
    require(re.search(r"\bUNITY_BEGIN\s*\(\s*\)", source) is not None,
            f"Unity suite must call UNITY_BEGIN(): {relative}")
    require(re.search(r"\bUNITY_END\s*\(\s*\)", source) is not None,
            f"Unity suite must call UNITY_END(): {relative}")
    require(re.search(r"\bvoid\s+setUp\s*\(\s*\)\s*\{", source) is not None,
            f"Unity suite must define setUp(): {relative}")
    require(re.search(r"\bvoid\s+tearDown\s*\(\s*\)\s*\{", source) is not None,
            f"Unity suite must define tearDown(): {relative}")
    require(re.search(r'#include\s*[<"]([^">]+\.cpp)[">]', source) is None,
            f"PlatformIO test must not include production .cpp files directly: {relative}")
    run_tests = len(re.findall(r"\bRUN_TEST\s*\(", source))
    require(run_tests > 0, f"Unity suite has no RUN_TEST cases: {relative}")
    unity_case_count += run_tests
    for token in legacy_tokens:
        require(token not in source,
                f"legacy Styr test-framework token {token!r} remains in PlatformIO test: {relative}")
    for token in ("stbi_write_png", "test_01_markdown", "shape-%"):
        require(token not in source,
                f"generated documentation/artifact side effect {token!r} remains in PlatformIO test: {relative}")

# Product test sources must not leak back into the simulator CMake test graph.
for cmake in SIM_TEST_ROOT.rglob("CMakeLists.txt"):
    text = cmake.read_text(encoding="utf-8")
    for forbidden in ("TestBootloader", "TestSequenceState", "TestLfoTrackEngine", "../test/"):
        require(forbidden not in text,
                f"product test reference {forbidden!r} found in {cmake.relative_to(ROOT)}")

# CMake/CTest is simulator-local.  There must be no second test tree in the
# repository root that can be confused with PlatformIO's canonical test/.
require(not (ROOT / "tests").exists(),
        "root tests/ directory is forbidden; simulator tests belong under src/simulator/tests/")
require((SIM_TEST_ROOT / "framework" / "UnitTest.h").is_file(),
        "simulator-local legacy UnitTest.h is missing")

require("TestSimulatorReboot" in SIM_CMAKE,
        "simulator lifecycle regression must remain in CTest")
require((SIM_TEST_ROOT / "unit" / "TestSimulatorReboot.cpp").is_file(),
        "missing simulator-specific TestSimulatorReboot source")
require("TestClockTimer" in SIM_CMAKE,
        "simulator ClockTimer elapsed-time regression must remain in CTest")
require((SIM_TEST_ROOT / "unit" / "TestClockTimer.cpp").is_file(),
        "missing simulator-specific TestClockTimer source")

if errors:
    print("Test ownership validation FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Test ownership validation OK:")
print(f" - PlatformIO Unity suites: {len(product_tests)}")
print(f"   - bootloader: {len(bootloader_tests)}")
print(f"   - shared core: {len(core_tests)}")
print(f"   - sequencer: {len(sequencer_tests)}")
print(f" - Unity RUN_TEST cases: {unity_case_count}")
print(" - native Unity dependency discovery: LDF enabled")
print(" - Windows native runtime: local toolchain DLL probe + transitive runtime staging enforced")
print(" - Simulator.cpp PNG dependency: stb_image_write implementation linked")
print(" - custom PlatformIO test runner: none")
print(" - CMake/CTest product-test references: none")
print(" - simulator CTest regressions: TestSimulatorReboot, TestClockTimer")
