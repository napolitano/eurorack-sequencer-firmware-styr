<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr documentation

This directory contains the maintained documentation for Styr. New documentation authored for Styr is licensed under **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** unless a file explicitly states otherwise. See [`LICENSE`](LICENSE).

The documentation tree is organized by audience rather than by build system:

- [`manual/`](manual/) — maintained user-facing operation, feature documentation, screen reference and its local image assets;
- [`development/`](development/) — architecture, firmware/simulator builds, formats and contributor workflows;
- [`testing/`](testing/) — test strategy, regression procedures and hardware verification;
- [`analysis/`](analysis/) — code audits, migration notes and design investigations;
- [`legacy/performer/`](legacy/performer/) — inherited PER|FORMER documentation kept for historical and technical reference.

Firmware installation and update procedures are maintained in the root-level [`README_FIRMWARE_UPDATE.md`](../README_FIRMWARE_UPDATE.md); the SD-card path is the normal end-user workflow.

## Licensing boundary

The CC BY-NC 4.0 documentation license applies only to Styr-authored documentation. It does **not** relicense source code, third-party material, or inherited PER|FORMER documentation. Files under `legacy/performer/` retain their upstream MIT provenance unless explicitly stated otherwise.


Documentation assets live next to the documentation area that owns them. User-manual images therefore live under `docs/manual/assets/`; a global `docs/assets/` bucket is intentionally not used.

From Munich with <img src="manual/assets/blue-heart.svg" alt="blue heart" width="14">
