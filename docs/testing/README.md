<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Testing

Styr uses the simulator for fast behavioral verification and retains embedded targets for hardware-specific validation. Unit and integration test source lives under `/tests`; this directory documents test strategy, regression procedures and release verification.

## Index

- [Bootloader verification](bootloader.md) — host unit tests, update-image invariants, memory-layout contracts and hard 32 KiB CI gates.

> [!IMPORTANT]
> The bootloader is release-critical and has stricter verification requirements than ordinary host-only components. See the dedicated bootloader verification document before changing its formatter, update format, flash layout or programming path.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
