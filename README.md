# Styr

## Acknowledgement

Styr exists because Simon Kallweit created **[PER|FORMER](https://github.com/westlicht/performer)**. The original firmware is a substantial piece of work: a deep Eurorack sequencer, a complete embedded platform, a simulator, a bootloader and a broad set of sequencing concepts that remain the technical foundation of this project. That contribution deserves clear and continuing credit.

Styr is not an attempt to take ownership of, obscure, or simply rebrand Simon's work. The goal is narrower and more practical: keep the firmware buildable, make the codebase easier to understand and maintain, modernize the development workflow, fix long-standing defects, and continue development where it is useful. Full-featured Eurorack sequencers are becoming increasingly uncommon; preserving and improving a capable open codebase is worthwhile.

The original PER|FORMER project remains the upstream source and historical reference for all inherited code. See [`PROVENANCE.md`](PROVENANCE.md) for how Styr records that relationship.

## Status

Styr is an experimental modernization of the PER|FORMER firmware. It is not yet a drop-in production replacement. The embedded build system, simulator, dependencies, source provenance and several runtime subsystems are being audited and modernized incrementally.

Current principles:

- preserve working sequencer behavior unless a change is deliberate and tested;
- keep the real firmware and simulator on the same sequencer implementation;
- retain the existing flash layout and SD-card update format while the legacy bootloader remains in use;
- modernize dependencies one at a time rather than changing the complete platform at once;
- keep inherited authorship and licenses explicit at file level.

## Repository map

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
├── docs/                   project documentation
├── .github/                contribution, security and repository metadata
├── platformio.ini          embedded build entry point
└── LICENSES/               license texts and provenance support
```

The important ownership rule is: **sequencer behavior belongs under `src/sequencer/` even though the simulator executes it.** The simulator is a development host, not a second implementation.

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

Build an SD-card update image:

```sh
pio run -e application -t sd-update
```

Flash the application through ST-Link:

```sh
pio run -e application -t upload
```

Detailed embedded build documentation is under [`docs/development/build/`](docs/development/build/). For installation on hardware, start with [`README_FIRMWARE_UPDATE.md`](README_FIRMWARE_UPDATE.md); SD-card update is the recommended end-user path and the ST-Link section contains the required hardware warning and connection order.

### Simulator

The simulator uses CMake and the same `src/sequencer/` implementation as the hardware firmware. Missing historical simulator dependencies are fetched automatically at their pinned Performer revisions and cached under `build/_deps/`.

On Windows with MSYS2 UCRT64:

```powershell
cd src\simulator
C:\msys64\ucrt64\bin\cmake.exe --preset windows-ucrt64-debug
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-ucrt64-debug
```

Run it from the repository root:

```powershell
.\build\simulator\windows-ucrt64-debug\styr_simulator.exe
```

See [`src/simulator/README.md`](src/simulator/README.md) for host prerequisites, tests and offline dependency behavior.

## Documentation

Start with [`docs/README.md`](docs/README.md). The reproducible manual screenshot workflow is documented separately in [`README_SCREENSHOTS.md`](README_SCREENSHOTS.md). Documentation is split by purpose:

- `docs/manual/` — maintained user manual, per-screen reference, feature guides and local manual image assets;
- `docs/development/` — architecture, build system, file formats and development workflows;
- `docs/testing/` — test strategy and verification procedures;
- `docs/analysis/` — audits, migration decisions and technical investigations;
- `docs/legacy/performer/` — inherited Performer documentation retained as historical reference.

## Contributing and security

Contribution guidance, issue templates, the project code of conduct and security-reporting instructions live under [`.github/`](.github/). This project is still changing quickly, so behavior changes should include focused regression tests wherever practical.

## Licensing and provenance

Inherited Performer source remains MIT-licensed. The root [`LICENSE`](LICENSE), [`LICENSES/MIT.txt`](LICENSES/MIT.txt) and [`PROVENANCE.md`](PROVENANCE.md) describe the software baseline and attribution model. Third-party code retains its own upstream licenses.

New Styr-authored documentation is licensed separately under **CC BY-NC 4.0**; see [`docs/LICENSE`](docs/LICENSE). Inherited Performer documentation under `docs/legacy/performer/` remains MIT-licensed.
