<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr Documentation Screenshots

Styr generates the LCD screenshots used by the end-user manual directly from the simulator. The process is deterministic and headless: it drives the real `SequencerApp` through defined UI states and writes the simulated LCD framebuffer to PNG files. No desktop screenshot API, manual crop, DPI scaling or image editor is involved.

Generated images belong only to [`docs/manual/assets/`](docs/manual/assets/). Every generated UI state has one matching Markdown reference below [`docs/manual/screens/`](docs/manual/screens/README.md).

## Capture architecture

```text
SequencerApp
    ↓
scripted simulator input + explicit model state
    ↓
LCD framebuffer (256 × 64)
    ↓
integer pixel replication
    ↓
PNG in docs/manual/assets/
```

The generator executable is implemented in:

```text
src/simulator/tools/manual_screenshots.cpp
```

Use direct model configuration for deterministic setup. Use simulated buttons, Shift/Page chords, function keys, encoder input and MIDI when the interaction itself is part of what the manual must explain.

### Startup and navigation synchronization

The firmware startup page is modal and remains active for its complete project-load interval. Documentation automation therefore **must not use a fixed short delay after reboot**. The generator polls the real simulator UI state and only sends page-navigation input after `StartupPage` has closed and a fresh main-page frame has been rendered. Screenshot calls are guarded as well: attempting to capture while the startup page is still active aborts the run instead of silently publishing an incorrect image.

This applies to the initial boot and every clean-session reboot between Note, Curve, MIDI/CV, LFO, generator, Song and System capture groups.

## Pixel-exact output

The native LCD framebuffer is **256 × 64 pixels**. Full-screen documentation screenshots default to **3×**, producing **768 × 192 pixel** PNG files. Header/footer reference crops use native 256×10 regions and therefore produce 768×30 images at the same 3× scale.

Scaling is restricted to integer factors from 1 through 8. Every source pixel becomes an exact N×N block. There is no interpolation, filtering or anti-aliasing.

The default is configured with:

```text
STYR_DOC_SCREENSHOT_SCALE=3
```

For example:

```powershell
cmake.exe --preset windows-ucrt64-debug -DSTYR_DOC_SCREENSHOT_SCALE=4
```

Do not resize generated screenshots in an image editor. Regenerate them at another integer scale instead.

## Windows prerequisites

The Windows simulator uses MSYS2 UCRT64. GLEW is required in addition to the compiler, CMake/Ninja and SDL2 dependencies:

```powershell
C:/msys64/usr/bin/pacman.exe -S --needed mingw-w64-ucrt-x86_64-glew
```

See [`src/simulator/README.md`](src/simulator/README.md) for the complete host setup.

## Generate the complete screenshot set

From `src/simulator`:

```powershell
cmake.exe --preset windows-ucrt64-debug
cmake.exe --build --preset windows-ucrt64-debug --target manual-screenshots
```

The target builds `styr_manual_screenshots`, creates `docs/manual/assets/` if needed and regenerates the complete PNG capture set.

> [!IMPORTANT]
> The screenshot target writes **images only**. It never creates, rewrites, reformats or deletes Markdown documents. Files below `docs/manual/` are maintained documentation sources and may contain hand-written edits that must survive every screenshot regeneration.

The capture set intentionally includes more than top-level pages. It covers:

- Note and Curve step-editor sublayers;
- their Shift direct-access functions;
- Note and Curve Quick Edit screens;
- LFO normal and Shift-layer functions, plus every supported waveform;
- Init Layer plus Euclidean, Random and Acid Bassline generator selection, parameters and Commit path;
- the remaining global, routing, monitoring, song and system states used by the manual.

LFO documentation states use the full **-5.00 V to +5.00 V** range unless the range itself is the subject of the example. This keeps the waveform preview legible instead of collapsing it around the centre line.

## Validate capture/documentation coverage

Run the repository check from the project root:

```powershell
python toolchain/check_manual_screenshots.py
```

The check enforces the one-to-one contract between:

```text
capture name in manual_screenshots.cpp
            ↕
Markdown file below docs/manual/screens/<area>/
            ↕
embedded docs/manual/assets/<capture-name>.png
```

It also checks for duplicate capture names and the required manual footer.

The Linux simulator CI explicitly builds `styr_manual_screenshots` before running the native tests, so compile errors in the documentation tool can no longer remain hidden behind `EXCLUDE_FROM_ALL`.

## Manual structure

```text
docs/manual/
├── README.md
├── assets/                         generated PNG files + manual-owned images
├── features/                       Styr-specific feature guides
│   ├── lfo/README.md
│   └── acid-bassline/README.md
└── screens/
    ├── README.md
    ├── interface/                    header and footer anatomy
    ├── global/
    ├── layout/
    ├── routing/
    ├── monitor/
    ├── note/
    ├── curve/
    ├── tracks/
    ├── lfo/
    ├── generators/
    ├── song/
    └── system/
```

Only directories that actually own images receive an `assets/` directory. End-user screenshot assets therefore remain under `docs/manual/assets/` rather than a repository-wide documentation asset bucket.

## Add a new documented state

1. Add a deterministic state to `src/simulator/tools/manual_screenshots.cpp`.
2. Use a stable lowercase semantic capture name.
3. Capture the interaction itself when modifiers, Commit/Revert, Quick Edit or similar behaviour is relevant.
4. Create or edit exactly one **manually maintained** Markdown document in the appropriate `docs/manual/screens/<area>/` directory. Base its initial explanation on the original PER|FORMER documentation where applicable and verify current behaviour against Styr code. Do not generate this Markdown from the screenshot tool.
5. Embed `../../assets/<capture-name>.png` from that screen document.
6. For a Styr-specific feature, update or add `docs/manual/features/<feature>/README.md`.
7. Run `python toolchain/check_manual_screenshots.py`.
8. Build the `manual-screenshots` target; it builds the generator and regenerates PNG files only.
9. Inspect the generated PNGs before committing them.

A screen document should explain what the state is, how it is reached, and what the relevant control does. It should not duplicate a complete feature guide.

## Review discipline

A changed generated PNG can mean either an intentional UI change or an unintended visual/state regression. Review screenshot diffs together with the corresponding Markdown changes.

Generated screenshots are committed documentation artifacts. They should remain readable without building Styr, while still being reproducible from the current codebase.

From Munich with <img src="docs/manual/assets/blue-heart.svg" alt="blue heart" width="14">
