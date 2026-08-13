<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr documentation

This directory contains the maintained public documentation for Styr. It is organized by **audience and purpose** so that end-user instructions, implementation details, verification procedures and inherited upstream material remain clearly separated.

> [!IMPORTANT]
> Markdown under the maintained Styr documentation tree is hand-maintained source material. Documentation tooling may regenerate owned assets such as screenshots, but must not overwrite the Markdown pages themselves.

## Index

- [Choose the right documentation area](#choose-the-right-documentation-area)
- [Documentation tree](#documentation-tree)
- [Placement rules](#placement-rules)
- [Operational entry points](#operational-entry-points)
- [Licensing boundary](#licensing-boundary)

## Choose the right documentation area

| Area | Audience | Contains | Entry point |
|---|---|---|---|
| `manual/` | End users | Operation, features, UI screens, controls and manual-owned assets. | [`manual/README.md`](manual/README.md) |
| `development/` | Developers | Architecture, build workflows, formats, documentation tooling, fonts and sequencer internals. | [`development/README.md`](development/README.md) |
| `testing/` | Developers / maintainers | Test strategy, regression procedures and verification guidance. | [`testing/README.md`](testing/README.md) |
| `legacy/performer/` | Historical reference | Inherited PER|FORMER technical documentation. | [`legacy/performer/`](legacy/performer/) |

## Documentation tree

```text
docs/
├── manual/
│   ├── features/           feature-oriented end-user guides
│   ├── screens/            per-screen and per-state UI reference
│   └── assets/             assets owned by the manual
├── development/
│   ├── architecture/       project/source architecture
│   ├── build/              build and toolchain workflows
│   ├── documentation/      documentation tooling
│   ├── fonts/              font toolchain
│   ├── formats/            persistent file formats
│   └── sequencer/          sequencer implementation details
├── testing/                verification and test documentation
└── legacy/performer/       inherited upstream documents
```

## Placement rules

1. **Describe operation in `manual/`.** If a user needs the information to operate the sequencer, it belongs in the manual rather than in a developer note.
2. **Keep feature guides and screen references separate.** Feature documents explain workflows and concepts. Screen documents explain concrete UI states, labels and controls.
3. **Keep Markdown stable.** Screenshot generation replaces PNG files only; it never regenerates or overwrites the corresponding Markdown pages.
4. **Put implementation details in `development/`.** Architecture, source ownership, formats and internal behavior belong there.
5. **Put verification in `testing/`.** Test strategy and procedures should remain distinct from implementation documentation.
6. **Keep inherited documentation isolated.** `legacy/performer/` preserves original PER|FORMER documentation as historical and technical reference rather than silently folding it into Styr-authored material.
7. **Keep assets local to their owner.** Create an `assets/` directory only where images are actually needed. User-manual screenshots therefore live under `manual/assets/`; there is intentionally no global `docs/assets/` directory.

> [!TIP]
> For a new user-visible feature, normally add a feature guide under `manual/features/`, dedicated screen/state documents under `manual/screens/`, and deterministic screenshots under `manual/assets/` where they help explain the UI.

## Operational entry points

Two frequently used procedures intentionally remain at repository root so they are directly visible from the GitHub landing page:

- [`README_FIRMWARE_UPDATE.md`](../README_FIRMWARE_UPDATE.md) — firmware installation/update, with SD card as the recommended end-user path and ST-Link safety guidance;
- [`README_SCREENSHOTS.md`](../README_SCREENSHOTS.md) — deterministic manual screenshot generation, scaling and validation.

The repository-level [`README.md`](../README.md) provides the overall project index and quick-start commands.

## Licensing boundary

New documentation authored for Styr is licensed under **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** unless a file explicitly states otherwise. See [`LICENSE`](LICENSE).

> [!NOTE]
> The documentation license does **not** relicense source code, third-party material or inherited PER|FORMER documentation. Files under `legacy/performer/` retain their upstream provenance and applicable upstream license.

From Munich with <img src="manual/assets/blue-heart.svg" alt="blue heart" width="14">
