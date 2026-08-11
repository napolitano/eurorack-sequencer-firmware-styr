<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Build workflows

This section is the developer entry point for building Styr. The repository deliberately keeps the embedded firmware and native simulator on separate build systems while sharing the actual sequencer implementation under `src/sequencer/`.

## Index

- [Embedded firmware](#embedded-firmware)
- [SD-card update bundle](#sd-card-update-bundle)
- [ST-Link development upload](#st-link-development-upload)
- [Native simulator](#native-simulator)
- [Tests](#tests)
- [Documentation screenshots](#documentation-screenshots)

## Embedded firmware

The embedded targets use PlatformIO. Install the pinned PlatformIO Core version from the repository root:

```sh
python -m pip install -r toolchain/requirements-platformio.txt
```

Build the main application:

```sh
pio run -e application
```

`platformio.ini` is the canonical embedded build entry point. Board definitions, linker files and supporting scripts live under `toolchain/`.

## SD-card update bundle

Build the application and package the update image expected by the existing bootloader:

```sh
pio run -e application -t sd-update
```

For installation on hardware, follow [`README_FIRMWARE_UPDATE.md`](../../../README_FIRMWARE_UPDATE.md). SD-card update is the recommended end-user installation path.

## ST-Link development upload

For development hardware connected through ST-Link:

```sh
pio run -e application -t upload
```

> [!CAUTION]
> The ST-Link header connects directly to the MCU without a protective interface. Read and follow the electrical warning and connection order in [`README_FIRMWARE_UPDATE.md`](../../../README_FIRMWARE_UPDATE.md) before attaching a programmer.

## Native simulator

The simulator uses CMake presets and is documented in detail in [`src/simulator/README.md`](../../../src/simulator/README.md).

Example for Windows/MSYS2 UCRT64:

```powershell
cd src\simulator
C:\msys64\ucrt64\bin\cmake.exe --preset windows-ucrt64-debug
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-ucrt64-debug
```

## Tests

Native unit and integration tests are built with the simulator and executed through CTest. The CI workflow uses the release preset on Linux:

```sh
cd src/simulator
cmake --preset release
cmake --build --preset release
ctest --preset release --output-on-failure
```

See [`docs/testing/README.md`](../../testing/README.md) for verification policy rather than build mechanics.

## Documentation screenshots

The deterministic manual screenshot generator is another simulator build target:

```powershell
cd src\simulator
cmake.exe --build --preset windows-ucrt64-debug --target manual-screenshots
```

The generator updates PNG assets only; Markdown remains hand-maintained. See [`README_SCREENSHOTS.md`](../../../README_SCREENSHOTS.md).

From Munich with <img src="../../manual/assets/blue-heart.svg" alt="blue heart" width="14">
