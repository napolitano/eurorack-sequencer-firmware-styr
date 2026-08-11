<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr project structure

## Purpose

The repository is organized for the person developing Styr, not around the historical build chain. The top-level distinction is intentionally obvious:

- `src/` contains Styr source code;
- `third_party/` contains imported source code;
- build systems adapt to that layout, not the other way around.

## Source tree

```text
src/
├── sequencer/              Styr product implementation
│   ├── engine/             sequencing, timing, routing, generators
│   ├── model/              projects, tracks, persistence, settings
│   ├── ui/                 pages, controllers, painters
│   └── platform/stm32/     real-hardware adapter
├── bootloader/             SD-update bootloader
├── hwconfig/               hardware configuration image
├── tester/                 hardware diagnostic firmware
├── simulator/              desktop/web host for the sequencer
├── tools/                  host-side source utilities
└── shared/                 genuinely generic cross-target support
    ├── core/               fs, gfx, math, MIDI, serialization, utilities
    └── os/                 common OS helpers
```

The main runtime relationship is:

```text
                     src/sequencer
              model / engine / UI / state
                          |
                 +--------+--------+
                 |                 |
       platform/stm32         src/simulator
                 |                 |
             STM32F405          host runtime

       src/shared = generic support used where needed
```

`src/sequencer` is the product implementation. The simulator is a host for that code, not its owner. `src/shared` is reserved for genuinely generic facilities used by multiple targets.

The low-level code in `src/shared/core` still has historical coupling to the platform API. It remains shared because it is used by multiple targets; that coupling can be reduced later without moving sequencer product logic back out of `src/sequencer`.

## Third-party tree

```text
third_party/
├── common/                 FatFs/STB used by shared runtime code
├── embedded/               FreeRTOS, libusbhost, libopencm3, bootloader deps
└── simulator/              NanoVG, SoLoud, RtMidi, pybind11, ...
```

Nothing moves into `src/` merely because Styr depends on it. Imported CMake modules and other build support with their own upstream license also live here. This boundary is the basis for dependency modernization and licensing/provenance work.

## Repository infrastructure and assets

```text
toolchain/
├── boards/                 PlatformIO board definitions
├── linker/                 STM32 linker scripts
└── scripts/                build, packaging and PlatformIO integration

assets/
└── fonts/                  editable/generated font source assets

docs/
├── manual/
│   ├── assets/             generated LCD screenshots + manual-owned images
│   ├── features/           Styr-specific user-facing feature READMEs
│   └── screens/            per-area indexes + one Markdown document per captured UI state
├── development/            architecture/build/formats/tooling
├── testing/                test and verification documentation
├── analysis/               audits and design investigations
└── legacy/performer/       inherited historical documentation
```

`toolchain/` is intentionally outside `src/`: developers normally work in the product sources, while board metadata, linker layouts and build glue are implementation details of the build environment. Host-side programs remain source code and therefore live under `src/tools/`; their input/output data lives under `assets/`. Documentation images are owned locally by the documentation area that uses them instead of being collected in a global `docs/assets/` directory.

## Build ownership

### Firmware

Owner: root `platformio.ini` plus `toolchain/scripts/platformio/` build glue.

PlatformIO owns compiler/toolchain selection, STM32F405 target configuration, linker scripts, application/bootloader/HWCONFIG/tester environments, ST-Link deployment and SD-update packaging. There is no embedded CMake or Make entry point.

### Simulator

Owner: `src/simulator/CMakeLists.txt`.

The simulator is a standalone CMake project. It compiles `src/sequencer` plus the small generic `src/shared` layer directly and has no dependency on the embedded toolchain. Native tests are part of its fast development loop.

### Font tools

Owner: `src/tools/fonts/CMakeLists.txt`.

Font conversion/editor utilities remain an independent host-side tool project.

## Development workflow

```text
edit src/sequencer
     |
     +--> src/simulator: CMake build + native tests
     |
     +--> PlatformIO firmware build
             |
             +--> ST-Link upload
             +--> UPDATE.DAT packaging
```

There is no generated shared package and no manual synchronization step between simulator and firmware.

## Deployment invariants

The structural migration does not intentionally change bootloader behavior, `UPDATE.DAT`, flash addresses, hardware-configuration layout or runtime behavior.

```text
Bootloader          0x08000000
Hardware config     0x08008000
Application         0x08010000
```

## Dependency policy

Dependencies are modernized one at a time. During that work versions stay pinned, legacy dependencies remain isolated below `third_party/`, simulator-only dependencies do not enter the embedded graph and embedded-only dependencies do not block simulator configuration.

The initial PlatformIO baseline deliberately retains the pinned Westlicht `libopencm3` source and GCC 6.3.1. Their replacement or upgrade is a later, independently testable change.

## Repository policy

Styr remains one monorepo. `src/sequencer`, the simulator and the shared support layer are not separate Git repositories or Git submodules. That preserves atomic commits across application logic, hardware integration, simulator behavior and tests.

From Munich with <img src="../../manual/assets/blue-heart.svg" alt="blue heart" width="14">
