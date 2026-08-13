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

---

Styr modernizes the build, simulator, tests, documentation and selected firmware functionality while retaining explicit provenance to the original PER|FORMER project.

## Why Styr?

**Styr** comes from the Swedish verb *styra*: to steer, guide, direct, or control. *Styr* is its present-tense form — roughly, *steers* or *controls*. The word reaches further back into the Germanic languages: Old Norse *stýra* carried much the same meaning, with relatives still visible in English *steer* and German *steuern*.

That felt appropriate for a sequencer.

Styr provides timing, structure, modulation, probability, routing and generative tools, but it is not intended to make musical decisions for you. The direction remains yours. It is there to steer a performance rather than define it.

The name is also deliberately short and functional: four letters for a device whose job is to keep a fairly complex musical system under control.

> [!IMPORTANT]
> Styr is an independent project. Westlicht and PER|FORMER identify the upstream hardware and firmware on which inherited parts of this repository are based; they are not Styr branding and are not presented as property of this project.

> [!WARNING]
> Styr is under active development and is not yet presented as a drop-in production replacement for the original firmware. Read the [firmware update guide](README_FIRMWARE_UPDATE.md) before installing it on hardware.

## Index

- [Acknowledgement](#acknowledgement)
- [Why Styr?](#why-styr)
- [Project goals](#project-goals)
- [Current status](#current-status)
- [Changes from PER|FORMER](#changes-from-performer)
  - [Fixed upstream bugs](#fixed-upstream-bugs)
  - [Implemented upstream TODOs](#implemented-upstream-todos)
  - [Improvements](#improvements)
  - [New features](#new-features)
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
| Bootloader | Critical / gated | The bootloader must link within 32 KiB; host tests, ABI checks and a hard CI size gate protect it. |
| Native simulator | Active | CMake-based; Windows UCRT64 and Linux workflows are maintained. |
| Product tests | Active | PlatformIO owns native bootloader, shared-core and sequencer/application tests; CI runs all 44 suites. |
| Manual screenshots | Active | Deterministic LCD captures generated from simulator state. |
| End-user manual | In progress | Screen and feature documentation is being expanded incrementally. |

> [!NOTE]
> The bootloader is release-critical and no longer allowed to fail silently in CI. Its ARM build must fit within 32 KiB and pass the dedicated bootloader verification gates. See [`docs/testing/bootloader.md`](docs/testing/bootloader.md).


## Changes from PER|FORMER

Styr keeps user-visible changes separate by type. The **Fixed upstream bugs** table contains only defects verified in the original Westlicht PER|FORMER v0.1.42 source. Styr-specific regressions are fixed as normal maintenance and are not attributed to upstream.

### Fixed upstream bugs

| Area | Inherited issue | What Styr does |
|---|---|---|
| External clock | The first real BPM measurement required a third edge because timestamp `0` also meant “no previous edge”. | Uses explicit edge-valid state, so the second edge already gives a real tempo measurement. |
| External clock | Initial slave BPM was strongly pulled toward zero by the startup filter. | Locks the first valid period immediately, then applies tracking to later measurements. |
| External clock | Alternating swing biased BPM because individual BPM values were smoothed instead of pulse periods. | Estimates tempo in the period domain and tracks swing phase separately. |
| External clock | Auto mode always used a 500 ms loss-of-clock timeout. | Uses an acquisition window and a timeout derived from the measured pulse period. |
| External clock | Phase recovery used unsigned time subtraction, so a future deadline could look massively overdue. | Uses wrap-safe signed/modular deadline comparisons. |
| External clock | Swing interpolation predicted the next interval from the interval that had just ended. | Learns the short/long pair and predicts the opposite interval. |
| External clock | Slave sub-tick deadlines were unsafe across the 32-bit microsecond rollover. | Uses rollover-safe time comparisons. |
| External clock | Stop/Continue could retain pending interpolated ticks. | Clears stale interpolation state before transport resumes. |
| External clock | Disabling the active MIDI/USB clock source could leave the clock stuck running. | Stops and releases the active slave source immediately. |
| Clock output | Pulse width was derived from master BPM even while slaved. | Derives pulse timing from the tempo source that is actually active. |
| Sequence | Single-step aligned PingPong could divide/modulo by zero. | Treats a one-step range as a stable one-step sequence. |
| Sequence | Single-step Free PingPong could temporarily leave its only valid step. | Keeps the playhead on the sole valid step. |
| Note track | Recording the first completed step could underflow its quantization window. | Uses a bounded first-step recording window without unsigned wraparound. |
| Note track | A positive gate could quantize to zero ticks at the fastest routed divisor. | Keeps every logically present gate at least one engine tick long. |
| Note track | Retrigger high pulses could quantize to zero ticks at small divisors. | Limits realized retrigger density to timing that can actually be represented. |
| Curve track | Free mode could jump phase when its divisor changed. | Keeps a local free-running phase and applies divisor changes without snapping to the transport grid. |
| Curve track | Next Pattern fill mixed current-pattern gate data with next-pattern curve data. | Evaluates gate and curve from the same next-pattern step. |
| Curve track | Next Pattern fill used the current pattern's shape-variation probability. | Uses the selected next-pattern step consistently for variation decisions. |
| Curve track | Step equality ignored gate and gate-probability data, so gate-only edits could appear unchanged. | Compares the complete Curve step state. |
| Curve track | A present Curve gate could quantize to a zero-tick pulse at the fastest divisor. | Keeps every present Curve gate at least one engine tick long. |

### Implemented upstream TODOs

| Upstream TODO / gap | What Styr does |
|---|---|
| Negative Note Gate Offset was explicitly left for future implementation. | Adds signed microtiming with deterministic look-ahead where the next event is knowable, and safe boundary fallback where it is not. |

### Improvements

| Area | What changed |
|---|---|
| External clock robustness | Isolated early/late pulses are distinguished from confirmed tempo changes, reducing reactions to double-triggers and single missing pulses. |
| Note microtiming | Future gate events are scheduled chronologically and generation-owned, so an old Gate-Off cannot cut short a newer pre-triggered gate. |
| Microtiming boundaries | Forward/Backward loop wraps can pre-trigger exactly; transport start, Reset Measure, linked tracks, pending pattern changes, dynamic Fill and non-deterministic modes use conservative boundary timing instead of guessing. |
| Simulator timing | Host clock timers now account for the full requested simulated wait interval, including the first millisecond after enable/re-enable. |
| Build and verification | Firmware tests are owned by PlatformIO/Unity; simulator-only tests remain under CMake/CTest, with hard bootloader and repository-cleanliness gates. |
| Source documentation | First-party APIs, state, constants and enums use structured US-English Doxygen documentation, enforced by a source-documentation quality gate and prepared for HTML/XML technical-reference generation. |

### New features

| Feature | What it adds |
|---|---|
| LFO track | Dedicated Free/Sync modulation track with eight waveforms, voltage range control, clipping and preview support. |
| Acid Bassline generator | Deterministic acid-style Note patterns with seed, root, length, density and legato controls plus preview/commit workflow. |
| Overview mini map | Compact indication of the active pattern segment in the overview. |
| Overview note labels | Note tracks can show note names directly in the overview. |
| Advanced Settings | Adds a dedicated system page for advanced configuration. |

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

PlatformIO is the canonical test runner for code that ships in the embedded product. Run the complete native product suite — bootloader, shared core and sequencer — from the repository root:

```sh
pio test -e test_product_native
```

The focused `test_bootloader_native` environment remains available for bootloader-only safety verification.

CMake/CTest is reserved for simulator-specific behavior. After configuring and building the simulator, run its host-only regression suite with:

```sh
cd src/simulator
ctest --preset release --output-on-failure
```

Test strategy, ownership rules and verification material belong under [`docs/testing/`](docs/testing/README.md).

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
| Source/API documentation rules | [`docs/development/documentation/README.md`](docs/development/documentation/README.md) | US-English Doxygen contract, technical-reference output and separation from end-user manual prose. |
| Understand persistent formats | [`docs/development/formats/README.md`](docs/development/formats/README.md) | Project, settings and user-scale file formats. |
| Work on sequencer internals | [`docs/development/sequencer/README.md`](docs/development/sequencer/README.md) | Track models, layers and song internals. |
| Verify behavior | [`docs/testing/README.md`](docs/testing/README.md) | Test strategy and verification procedures. |
| Consult upstream historical docs | [`docs/legacy/performer/`](docs/legacy/performer/) | Original PER|FORMER technical documents retained as historical reference. |

### Agent-assisted manual maintenance

Styr uses repository-scoped AI agents to help analyze, draft, review, and edit the end-user manual. The process is deliberately transparent and review-driven: agent-produced documentation is version-controlled like any other source change, is checked against the implementation, tests, and deterministic screenshots, and requires human review before it becomes part of a release. Release documentation is built from the reviewed repository sources; agents are not used to rewrite the manual at release time.

Documentation-agent work is maintainer-triggered. Public issues or comments are not treated as authorization to run an agent or modify the manual. See [`docs/development/documentation/README.md`](docs/development/documentation/README.md) for the maintenance and authorization model.

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
└── legacy/performer/       inherited upstream documentation kept unchanged where possible
```

The placement rules are:

1. **End-user behavior belongs in `docs/manual/`.** A feature or screen should be understandable without reading implementation notes.
2. **Every relevant UI state can have its own Markdown document.** Screenshot regeneration replaces only the corresponding PNG asset, never the Markdown source.
3. **Feature guides and screen references are complementary.** Feature guides explain workflows and concepts; screen pages document concrete UI states and controls.
4. **Developer material belongs in `docs/development/`.** Architecture, formats, source ownership and implementation details should not leak into the user manual unless needed to operate the module.
5. **Verification belongs in `docs/testing/`.** Test policy and procedures stay separate from implementation design notes.
6. **Inherited PER|FORMER documents stay under `docs/legacy/performer/`.** They remain historical/upstream references and retain their original provenance.
7. **Assets live with the documentation area that owns them.** There is deliberately no generic `docs/assets/` bucket; manual images live in `docs/manual/assets/`.
8. **Root-level operational READMEs are exceptional entry points.** Firmware update and screenshot-generation procedures stay at repository root because they are frequently used directly from the GitHub landing page.

> [!TIP]
> When adding a new feature, start with a feature README under `docs/manual/features/`, add dedicated screen references under `docs/manual/screens/` for relevant UI states, and add deterministic screenshots to `docs/manual/assets/` where they materially improve the explanation.

## Repository layout

```text
styr/
├── src/                    first-party source code
│   ├── sequencer/          main Styr sequencer implementation
│   ├── simulator/          desktop host + simulator-only CMake/CTest tests
│   ├── bootloader/         SD-card firmware updater
│   ├── hwconfig/           hardware configuration image
│   ├── tester/             hardware diagnostics firmware
│   ├── shared/             genuinely cross-target support code
│   └── tools/              host-side source utilities
├── assets/                 first-party build/runtime source assets
├── test/                   PlatformIO + Unity product tests
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

Inherited PER|FORMER source remains MIT-licensed. The root [`LICENSE`](LICENSE) and [`LICENSES/MIT.txt`](LICENSES/MIT.txt), together with file-level authorship/SPDX headers, preserve the software licensing and attribution boundary. Third-party code retains its own upstream licenses.

New Styr-authored documentation is licensed separately under **CC BY-NC 4.0**; see [`docs/LICENSE`](docs/LICENSE). Inherited PER|FORMER documentation under `docs/legacy/performer/` retains its upstream provenance and license.

From Munich with <img src="docs/manual/assets/blue-heart.svg" alt="blue heart" width="14">
