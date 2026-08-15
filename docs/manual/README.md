<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr User Manual

The maintained end-user documentation for Styr lives here. Markdown is hand-maintained source material; the screenshot pipeline regenerates PNG files only and never rewrites these documents.

LCD images are stored only in [`assets/`](assets/). Every captured UI state has a dedicated, hand-maintained Markdown reference page below [`screens/`](screens/README.md); related states are grouped by functional area so the manual remains navigable as coverage grows. Screenshot regeneration updates PNG files only after a complete successful capture run.

## Reader journey (guide)

- [Device Overview and Navigation](guide/device-overview.md)
- [Core Concepts](guide/core-concepts.md)
- [Getting Started](guide/getting-started.md)
- [Editing Sequences](guide/editing-sequences.md)
- [Recording Sequences](guide/recording-sequences.md)
- [Patterns](guide/patterns.md)
- [Song Mode](guide/song-mode.md)
- [Performance Mode](guide/performance-mode.md)
- [Advanced Timing and Modulation](guide/advanced-timing-and-modulation.md)
- [Advanced Routing, MIDI, and CV](guide/advanced-routing-midi-cv.md)
- [Projects, Maintenance, and System Tasks](guide/projects-and-system.md)

## Feature guides

- [LFO Track](features/lfo/README.md)
- [Acid Bassline Generator](features/acid-bassline/README.md)

## Display anatomy

- [Header](screens/interface/header.md)
- [Footer](screens/interface/footer.md)

## Screen reference

- [Display Layout](screens/interface/README.md)
- [Global and Project](screens/global/README.md)
- [Layout](screens/layout/README.md)
- [Routing, MIDI and Scales](screens/routing/README.md)
- [Monitoring](screens/monitor/README.md)
- [Note Track](screens/note/README.md)
- [Curve Track](screens/curve/README.md)
- [Other Track Modes](screens/tracks/README.md)
- [LFO Track](screens/lfo/README.md)
- [Generators](screens/generators/README.md)
- [Song](screens/song/README.md)
- [System](screens/system/README.md)

## Release history

- [Release History](releases/README.md)

## Screenshot convention

The native LCD framebuffer is 256×64 pixels. Manual captures default to **3×**, producing 768×192 pixel PNG files. Scaling is always an integer multiple and uses direct nearest-neighbor pixel replication; screenshots must never be resampled with anti-aliasing. See [`README_SCREENSHOTS.md`](../../README_SCREENSHOTS.md) for generation, validation, and review workflow.

## Publication pipeline

Markdown in this directory is the maintained source for the user manual. Publication is a deterministic formatting step and does not ask an AI agent to rewrite the manual during a release.

The current publication path is:

```text
Markdown -> Pandoc -> styled ODT -> LibreOffice Writer -> PDF
```

The Writer layout is defined by [`style/styr-reference.odt`](style/styr-reference.odt) and [`style/theme.toml`](style/theme.toml). The reference document uses the supplied Styr manual design: the narrow 99.01 x 210.01 mm page, Ubuntu/Ubuntu Light typography, compact spacing, footer/page numbering and black Styr accent. Semantic warning/status colors remain independent of the primary accent.

Font binaries are deliberately not stored in this repository. Strict CI/release builds fetch the pinned Ubuntu classic font archive into the ephemeral build environment, verify its checksum, install it for that run, and fail if Ubuntu/Ubuntu Light cannot be resolved. Debug builds may explicitly permit substitution; release builds may not.

Release artifact names are derived from the firmware tag and the locale manifest. For firmware `0.1.4`, the current English output is `styr-user-manual.en.0.1.4.pdf`; the matching ODT is retained as an intermediate/debug artifact. Future enabled locales use the same pipeline and their ISO 639-1 language code without adding language-specific build logic.
