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

For example, the dedicated headless preset can be overridden without enabling the desktop frontend:

```powershell
cmake.exe --preset windows-ucrt64-manual-screenshots -DSTYR_DOC_SCREENSHOT_SCALE=4
```

Do not resize generated screenshots in an image editor. Regenerate them at another integer scale instead.

## Host prerequisites

Manual screenshot generation uses a dedicated headless build. It needs the native compiler plus CMake/Ninja, but **not** SDL2, OpenGL, GLEW, X11, Wayland or a display server. Those dependencies belong only to the interactive desktop simulator. On Windows the headless preset uses the same MSYS2 UCRT64 GCC toolchain.

See [`src/simulator/README.md`](src/simulator/README.md) for the additional prerequisites required when building the interactive simulator.

## Generate the complete screenshot set

From the repository root, use the canonical helper:

```powershell
python toolchain/regenerate_manual_screenshots.py
```

It selects `manual-screenshots` on Linux/macOS and `windows-ucrt64-manual-screenshots` on Windows, configures with the interactive frontend and dependency fetching disabled, runs the canonical `manual-screenshots` target, and then validates the complete asset set.

The underlying Windows commands are:

```powershell
cd src/simulator
cmake.exe --preset windows-ucrt64-manual-screenshots
cmake.exe --build --preset windows-ucrt64-manual-screenshots --target manual-screenshots
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

CI validates the screenshot path twice: the normal Linux simulator job still builds `styr_manual_screenshots` inside the full desktop graph, while a separate headless job runs the complete regeneration with no display and no graphics development packages. The headless job regenerates the corpus a second time and requires all 126 PNG SHA-256 hashes to remain identical. It also inspects the built executable and rejects SDL/OpenGL/GLEW/X11/Wayland linkage.

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

## Documentation-agent regeneration contract

The agentic documentation modes treat screenshots as part of the documentation state, not as optional supporting files.

For `full-regeneration`:

1. the Orchestrator runs `python3 toolchain/regenerate_manual_screenshots.py`, which drives the canonical `manual-screenshots` target, before documentation analysis;
2. the Analyst and Manual SME work from the freshly rendered UI states;
3. the Manual SME or UX/IA Reviewer may improve deterministic capture definitions when a technically correct state is a poor instructional example;
4. after authoring and UX/IA review, the Orchestrator regenerates the **complete** screenshot corpus again;
5. `python3 toolchain/check_manual_screenshots.py --require-assets` must pass before Technical Review.

For `incremental-release-sync`, affected capture definitions are updated with the documentation change and the complete corpus is regenerated once before Technical Review. This intentionally favors a coherent current screenshot set over mixing generated assets from different documentation runs.

A documentation sync must not claim screenshot regeneration merely because capture mappings validate. The canonical generator must actually run successfully.

## Review discipline

A changed generated PNG can mean either an intentional UI change or an unintended visual/state regression. Review screenshot diffs together with the corresponding Markdown changes.

Generated screenshots are committed documentation artifacts. They should remain readable without building Styr, while still being reproducible from the current codebase.

From Munich with <img src="docs/manual/assets/blue-heart.svg" alt="blue heart" width="14">

### Simulator reboot isolation

The documentation generator deliberately resets the simulated firmware between complex capture groups. Simulator platform drivers register as target-input observers and now unregister themselves in their destructors. This RAII lifecycle is required: keeping observers from a destroyed `SequencerApp` would leave dangling pointers and can cause native access violations during later capture groups.

`TestSimulatorReboot` exercises repeated target recreation as a regression test for this contract.


## Section isolation

The canonical `manual-screenshots` target does not generate the entire manual in one long-lived simulator process. It invokes the capture tool separately for `global`, `note`, `curve`, `midi-cv`, `lfo`, `generators`, `song`, and `system`.

This is intentional. Each section starts with a fresh `SequencerApp` and simulator process, so modal UI state, model state, observer lifetimes, and driver state cannot leak from one documentation group into the next. The shared staging directory is published to `docs/manual/assets/` only after every section succeeds.

A single section can be reproduced directly for debugging:

```powershell
.\build\simulator\windows-ucrt64-manual-screenshots\styr_manual_screenshots.exe `
  .\build\simulator\windows-ucrt64-manual-screenshots\manual-screenshots-debug `
  3 `
  generators
```

Valid sections are `global`, `note`, `curve`, `midi-cv`, `lfo`, `generators`, `song`, and `system`. Omitting the section retains the developer-oriented `all` mode.

