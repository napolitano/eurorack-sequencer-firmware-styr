# Contributing to Styr

Styr is in an active modernization phase. Contributions are welcome, but changes should be narrow enough to review and verify without obscuring behavior changes behind unrelated refactoring.

## Before changing code

1. Check whether the behavior is inherited PER|FORMER behavior or a Styr-specific change.
2. Keep sequencer product logic under `src/sequencer/`; simulator use does not make code `shared`.
3. Do not modify `third_party/` unless the task is explicitly a dependency update or license/provenance correction.
4. Preserve the flash map and `UPDATE.DAT` format unless a change explicitly targets them.
5. For persistent model changes, document the file-format/version impact before merging.

## Build and test

At minimum, firmware changes should build:

```sh
pio run -e application
```

Simulator-visible behavior should also be checked with the host build and relevant tests. See `docs/testing/` and `src/simulator/README.md`.

Before a pull request that touches build infrastructure, run the affected PlatformIO environments or CMake presets rather than relying only on editor diagnostics.

## Source headers and provenance

First-party source files carry a short file header describing purpose, authorship and license. Do not remove upstream authorship. If a previously inherited file is materially changed for Styr, add Axel Napolitano as a Styr contributor while retaining Simon Kallweit's original attribution.

Software remains under the license stated by the file and repository provenance records. New Styr documentation is CC BY-NC 4.0; inherited PER|FORMER documentation remains MIT.

## Pull requests

Keep pull requests focused. Explain:

- what problem is being solved;
- whether behavior changes;
- how it was tested;
- whether persistence, bootloader, hardware compatibility or licensing is affected.

Do not combine dependency upgrades with unrelated feature work.
