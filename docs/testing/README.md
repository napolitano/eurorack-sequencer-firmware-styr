<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Testing

Styr deliberately separates **product-code tests** from **simulator tests** by build-system ownership.

- **PlatformIO owns every test whose subject is code that is also built by PlatformIO for the embedded product**: bootloader, shared core, sequencer/application logic and, as those areas gain tests, HW-config and tester code.
- **CMake/CTest owns simulator-specific tests only**: desktop simulator lifecycle, host frontend/integration behavior and simulator tooling.

This boundary is enforced by `toolchain/check_test_ownership.py` and by CI. Product tests must not be reintroduced into the CMake test graph merely because they execute natively on a development machine.

## Index

- [Bootloader verification](bootloader.md) — PlatformIO native unit tests, update-image invariants, memory-layout contracts and hard 32 KiB ARM gates.

## Running product tests

From the repository root:

```sh
pio test -e test_product_native
```

`test_product_native` is the complete native product suite: bootloader, shared core and sequencer/application code. PlatformIO still builds each suite as an independent executable; the native adapter therefore keeps the link graph suite-local (bootloader support only for bootloader suites, normal product support for core/sequencer suites). The focused `test_bootloader_native` environment remains available for bootloader-only verification and release-safety work. Both environments use PlatformIO's native platform with Unity, so they execute quickly on the host while remaining under the same build/test orchestrator as the embedded targets. The native test environments deliberately keep PlatformIO's Library Dependency Finder enabled (`lib_ldf_mode = chain`): Unity is supplied by PlatformIO as a test-library dependency, and disabling LDF prevents `<unity.h>` from being added to the compiler include path. The embedded production environments may continue to use manual/off LDF because their dependency graph is assembled explicitly.

> [!NOTE]
> Always select one of the native test environments explicitly. A bare `pio test` follows the project's default `application` environment, which is an embedded firmware build and intentionally has no Unity test framework configured.

Run an individual suite by filtering its path relative to `test/`:

```sh
pio test -e test_product_native --filter "bootloader/test_format"
pio test -e test_product_native --filter "sequencer/test_lfo_track_engine"
```

The native environments use the host GCC toolchain. On Windows, Styr resolves the same MSYS2/UCRT64 GCC installation used by the native simulator and injects its tool directories into the PlatformIO SCons environment. The default location is `C:/msys64/ucrt64/bin`; custom installations can set `STYR_NATIVE_TOOLCHAIN_BIN` (or `STYR_MSYS2_ROOT`). To avoid accidental loading of an incompatible MinGW runtime from the developer's global `PATH`, the adapter inspects each native test executable with `objdump` and copies the transitive DLL closure belonging to the selected UCRT64 toolchain directly beside `program.exe`. A small `std::vector` probe validates the same local-runtime arrangement before the suites start.

Tests are organized using PlatformIO's test hierarchy below the repository root `test/` directory:

```text
test/
├── bootloader/
│   └── test_*/
├── core/
│   └── .../test_*/
└── sequencer/
    └── test_*/
```

Each `test_*` directory is an independent Unity test executable with its own `main()`. Suites use the conventional Unity lifecycle (`setUp()`, `tearDown()`, `UNITY_BEGIN()`, `RUN_TEST(...)`, `UNITY_END()`) and Unity assertion macros directly. PlatformIO uses its built-in Unity integration for all product-code suites; no Styr-specific PlatformIO test runner or assertion framework sits between PlatformIO and the tests. This keeps test discovery, result parsing and assertion semantics conventional and portable.

Product tests include production headers, not production `.cpp` files. Production sources are compiled through the PlatformIO native source graph into static support libraries; each independent Unity executable therefore extracts only the production objects it actually references. This matches normal compilation/link boundaries and prevents an unrelated subsystem from imposing its host stubs or platform dependencies on a focused test suite. Product unit tests must also be side-effect free with respect to the repository: documentation renderers and file-producing helpers do not belong in `test/`. `toolchain/check_repository_cleanliness.py` rejects the old curve-shape PNG generator pattern and generated root artifacts.

`toolchain/scripts/platformio/native_product_sources.py` is build integration only: it selects the production sources and deterministic host I/O fakes required by the native environments. It does not discover tests, provide assertions, execute suites or parse results; those responsibilities stay with PlatformIO and Unity.

## Running simulator tests

Simulator-specific regressions remain in CMake/CTest:

```sh
cd src/simulator
cmake --preset release
cmake --build --preset release
ctest --preset release --output-on-failure
```

The CTest suite must not become a second owner of embedded/product unit tests. At present `TestSimulatorReboot` covers target recreation and host-side observer/callback lifecycle behavior, while `TestClockTimer` covers simulator-only elapsed-time accounting used by host clock tests.

## Ownership gate

Run the structural policy check directly with:

```sh
python toolchain/check_test_ownership.py
```

The gate verifies the PlatformIO Unity environments, expected product-test hierarchy and suite lifecycle, CI invocation, absence of legacy/custom PlatformIO test frameworks, absence of direct production `.cpp` inclusion, and absence of product tests from CMake/CTest.

> [!IMPORTANT]
> The bootloader is release-critical and has stricter verification requirements than ordinary product code. See the dedicated bootloader verification document before changing its formatter, update format, flash layout or programming path.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
