# Styr

[![CI](https://github.com/napolitano/eurorack-sequencer-firmware-styr/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/napolitano/eurorack-sequencer-firmware-styr/actions/workflows/ci.yml)
[![Code license: MIT](https://img.shields.io/badge/code%20license-MIT-blue.svg)](LICENSE)
[![Documentation: CC BY-NC 4.0](https://img.shields.io/badge/docs-CC%20BY--NC%204.0-lightgrey.svg)](docs/LICENSE)
![Target: STM32F405RG](https://img.shields.io/badge/target-STM32F405RG-03234B.svg?logo=stmicroelectronics&logoColor=white)
![Firmware build: PlatformIO](https://img.shields.io/badge/firmware-PlatformIO-F5822A.svg?logo=platformio&logoColor=white)
![Simulator build: CMake](https://img.shields.io/badge/simulator-CMake-064F8C.svg?logo=cmake&logoColor=white)

## Acknowledgement

Styr exists because Simon Kallweit created **[Westlicht PER|FORMER](https://github.com/westlicht/performer)**. The hardware concept, original firmware and the technical foundation this fork builds on are his work. I am genuinely grateful that he made PER|FORMER available as an open project and gave others something substantial to learn from and continue working with.

Styr is an independent continuation and modernization of that foundation. It is not an attempt to blur, replace or take ownership of Simon's authorship; keeping that provenance clear is part of the project itself.


## Why “Styr”?

**Styr** comes from the Swedish verb *styra*: to steer, guide, direct, or control. *Styr* is its present-tense form - literally, something that *steers*.

The word reaches further back into the Germanic languages. Old Norse *stýra* carried much the same meaning, and its relatives can still be found in English *steer* and German *steuern*.

That felt appropriate for a sequencer.

Styr is not meant to make musical decisions for you. It provides timing, structure, modulation, probability, routing, and generative tools — but the direction remains yours. It is there to steer a performance rather than define it.

The name is also deliberately short and functional: four letters for a device whose job is to keep a fairly complex musical system under control.

---

Styr modernizes the build, simulator, tests, documentation and selected firmware functionality while retaining explicit provenance to the original PER|FORMER project.

> [!IMPORTANT]
> Styr is an independent project. Westlicht and PER|FORMER identify the upstream hardware and firmware on which inherited parts of this repository are based; they are not Styr branding and are not presented as property of this project.

> [!WARNING]
> Styr is under active development and is not yet presented as a drop-in production replacement for the original firmware. Read the [firmware update guide](README_FIRMWARE_UPDATE.md) before installing it on hardware.

## Index

- [Acknowledgement](#acknowledgement)
- [Project goals](#project-goals)
- [Current status](#current-status)
- [Quick start](#quick-start)
  - [Firmware](#firmware)
  - [Simulator](#simulator)
  - [Tests](#tests)
  - [Manual screenshots](#manual-screenshots)
- [Documentation](#documentation)
  - [Documentation index](#documentation-index)
  - [How the documentation is organized](#how-the-documentation-is-organized)
- [Repository layout](#repository-layout)
- [Development principles](#development-principles)
- [Contributing and security](#contributing-and-security)
- [Licensing and provenance](#licensing-and-provenance)

## Project goals

Styr focuses on preserving a capable Eurorack sequencing platform while making continued development practical. The work is deliberately incremental:

- keep working sequencer behavior stable unless a change is intentional and tested;
- keep firmware and simulator on the same `src/sequencer/` implementation;
- provide reproducible PlatformIO and CMake build workflows;
- retain the established flash layout and SD-card update path while the legacy bootloader remains in service;
- improve automated testing and deterministic documentation tooling;
- modernize dependencies and subsystems one at a time instead of replacing the whole platform at once;
- keep inherited authorship, provenance and third-party licensing explicit.

## Current status

| Area | Status | Notes |
|---|---|---|
| Application firmware | Active | PlatformIO build is part of CI. |
| SD-card update bundle | Active | Recommended installation path for end users. |
| Bootloader | Migration blocker | The legacy 32 KiB size constraint is still under investigation. |
| Native simulator | Active | CMake-based; Windows UCRT64 and Linux workflows are maintained. |
| Native tests | Active | Unit and integration tests run in CI. |
| Manual screenshots | Active | Deterministic LCD captures generated from simulator state. |
| End-user manual | In progress | Screen and feature documentation is being expanded incrementally. |

> [!NOTE]
> The bootloader build is intentionally allowed to fail in CI while its remaining size delta is investigated. See [`docs/analysis/BOOTLOADER_SIZE.md`](docs/analysis/BOOTLOADER_SIZE.md).

## Quick start

### Firmware

Install the pinned PlatformIO Core version:

```sh
python -m pip install -r toolchain/requirements-platformio.txt
```

Build the application:

```sh
pio run -e application
```

Build the SD-card update image:

```sh
pio run -e application -t sd-update
```

Flash through ST-Link:

```sh
pio run -e application -t upload
```

> [!CAUTION]
> ST-Link is a development/service path, not the recommended end-user update method. The programming header connects directly to the MCU and has no protective interface. Follow the connection order and electrical warnings in [`README_FIRMWARE_UPDATE.md`](README_FIRMWARE_UPDATE.md).

### Simulator

The simulator executes the same sequencer implementation used by the hardware firmware.

On Windows with MSYS2 UCRT64:

```powershell
cd src\simulator
C:\msys64\ucrt64\bin\cmake.exe --preset windows-ucrt64-debug
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-ucrt64-debug
```

Run from the repository root:

```powershell
.\build\simulator\windows-ucrt64-debug\styr_simulator.exe
```

Host prerequisites and additional presets are documented in [`src/simulator/README.md`](src/simulator/README.md).

### Tests

Run the native simulator test suite after configuring and building the simulator:

```sh
cd src/simulator
ctest --preset release --output-on-failure
```

Test strategy and verification material belongs under [`docs/testing/`](docs/testing/README.md).

### Manual screenshots

Generate the maintained LCD documentation screenshots on Windows:

```powershell
cd src\simulator
cmake.exe --build --preset windows-ucrt64-debug --target manual-screenshots
```

The generator updates PNG assets only. It does **not** generate or overwrite the hand-maintained Markdown documentation.

See [`README_SCREENSHOTS.md`](README_SCREENSHOTS.md) for the capture model, scaling rules, validation and review workflow.

## Documentation

The documentation is intentionally organized by **audience and purpose**, not by whichever source directory happens to implement a feature. The main entry point is [`docs/README.md`](docs/README.md).

### Documentation index

| Need | Start here | Purpose |
|---|---|---|
| Use Styr | [`docs/manual/README.md`](docs/manual/README.md) | Maintained end-user manual, features, UI screens and controls. |
| Install/update firmware | [`README_FIRMWARE_UPDATE.md`](README_FIRMWARE_UPDATE.md) | SD-card update workflow and ST-Link service/development procedure. |
| Regenerate manual images | [`README_SCREENSHOTS.md`](README_SCREENSHOTS.md) | Deterministic screenshot generation and validation. |
| Understand the architecture | [`docs/development/README.md`](docs/development/README.md) | Build, architecture, formats, sequencer internals and developer workflows. |
| Understand persistent formats | [`docs/development/formats/README.md`](docs/development/formats/README.md) | Project, settings and user-scale file formats. |
| Work on sequencer internals | [`docs/development/sequencer/README.md`](docs/development/sequencer/README.md) | Track models, layers and song internals. |
| Verify behavior | [`docs/testing/README.md`](docs/testing/README.md) | Test strategy and verification procedures. |
| Read technical investigations | [`docs/analysis/README.md`](docs/analysis/README.md) | Audits, migration notes and unresolved technical investigations. |
| Consult upstream historical docs | [`docs/legacy/performer/`](docs/legacy/performer/) | Original PER|FORMER technical documents retained as historical reference. |
| Check authorship and provenance | [`PROVENANCE.md`](PROVENANCE.md) | Relationship between inherited PER|FORMER code and Styr changes. |

### How the documentation is organized

```text
docs/
├── manual/                 end-user documentation
│   ├── features/           feature-oriented guides
│   ├── screens/            one maintained Markdown page per relevant UI state
│   └── assets/             manual-owned images, including generated LCD captures
├── development/            implementation and contributor documentation
│   ├── architecture/       source ownership and system structure
│   ├── build/              embedded build workflows
│   ├── documentation/      documentation tooling internals
│   ├── fonts/              font conversion and editing toolchain
│   ├── formats/            persistent file formats
│   └── sequencer/          sequencer implementation details
├── testing/                test strategy and verification procedures
├── analysis/               audits, migration work and technical investigations
└── legacy/performer/       inherited upstream documentation kept unchanged where possible
```

The placement rules are:

1. **End-user behavior belongs in `docs/manual/`.** A feature or screen should be understandable without reading implementation notes.
2. **Every relevant UI state can have its own Markdown document.** Screenshot regeneration replaces only the corresponding PNG asset, never the Markdown source.
3. **Feature guides and screen references are complementary.** Feature guides explain workflows and concepts; screen pages document concrete UI states and controls.
4. **Developer material belongs in `docs/development/`.** Architecture, formats, source ownership and implementation details should not leak into the user manual unless needed to operate the module.
5. **Verification belongs in `docs/testing/`.** Test policy and procedures stay separate from implementation design notes.
6. **Investigations belong in `docs/analysis/`.** These documents may describe unresolved findings and should not be treated as normative user documentation.
7. **Inherited PER|FORMER documents stay under `docs/legacy/performer/`.** They remain historical/upstream references and retain their original provenance.
8. **Assets live with the documentation area that owns them.** There is deliberately no generic `docs/assets/` bucket; manual images live in `docs/manual/assets/`.
9. **Root-level operational READMEs are exceptional entry points.** Firmware update and screenshot-generation procedures stay at repository root because they are frequently used directly from the GitHub landing page.

> [!TIP]
> When adding a new feature, start with a feature README under `docs/manual/features/`, add dedicated screen references under `docs/manual/screens/` for relevant UI states, and add deterministic screenshots to `docs/manual/assets/` where they materially improve the explanation.

## Repository layout

```text
styr/
├── src/                    first-party source code
│   ├── sequencer/          main Styr sequencer implementation
│   ├── simulator/          desktop host for the sequencer
│   ├── bootloader/         SD-card firmware updater
│   ├── hwconfig/           hardware configuration image
│   ├── tester/             hardware diagnostics firmware
│   ├── shared/             genuinely cross-target support code
│   └── tools/              host-side source utilities
├── assets/                 first-party build/runtime source assets
├── tests/                  unit, integration and simulator tests
├── third_party/            imported source governed by upstream licenses
├── toolchain/              board, linker and build-system integration
├── docs/                   maintained documentation tree
├── .github/                CI, contribution, security and repository metadata
├── platformio.ini          embedded build entry point
└── LICENSES/               license texts and provenance support
```

> [!IMPORTANT]
> Sequencer behavior belongs under `src/sequencer/` even when it is exercised through the simulator. The simulator is a host for the real sequencer implementation, not a parallel firmware implementation.

## Development principles

- CLI-first and editor-independent build workflows are canonical.
- VSCodium/VS Code configuration may provide convenience, but must not become a build requirement.
- New behavior should receive focused regression coverage where practical.
- Third-party code remains visibly separated under `third_party/`.
- Documentation changes should preserve the distinction between inherited upstream facts, current Styr behavior and unresolved analysis.
- Generated artifacts must not overwrite hand-maintained documentation sources.

## Contributing and security

Repository policies live under [`.github/`](.github/):

- [`CONTRIBUTING.md`](.github/CONTRIBUTING.md) — contribution workflow;
- [`CODE_OF_CONDUCT.md`](.github/CODE_OF_CONDUCT.md) — community expectations;
- [`SECURITY.md`](.github/SECURITY.md) — security reporting;
- [`SUPPORT.md`](.github/SUPPORT.md) — support scope;
- [`GOVERNANCE.md`](.github/GOVERNANCE.md) — project governance.

## Licensing and provenance

Inherited PER|FORMER source remains MIT-licensed. The root [`LICENSE`](LICENSE), [`LICENSES/MIT.txt`](LICENSES/MIT.txt) and [`PROVENANCE.md`](PROVENANCE.md) describe the software baseline and attribution model. Third-party code retains its own upstream licenses.

New Styr-authored documentation is licensed separately under **CC BY-NC 4.0**; see [`docs/LICENSE`](docs/LICENSE). Inherited PER|FORMER documentation under `docs/legacy/performer/` retains its upstream provenance and license.

From Munich with <img src="docs/manual/assets/blue-heart.svg" alt="blue heart" width="14">
