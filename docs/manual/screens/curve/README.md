<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Curve Track Screen Reference

A Curve track uses the same 64-step sequencing structure as a Note track, but each step describes a CV shape between minimum and maximum levels. It can also emit gates and apply shape variation probabilistically.

## Track and sequence pages

- [Curve Track](curve-track.md) — track-wide playback and output behaviour.
- [Curve Sequence](curve-sequence.md) — First Step, Last Step, Run Mode, Divisor, Reset Measure and voltage Range.

## Step layers

| Function key | Layer group |
| --- | --- |
| `F1` | Shape → Shape Variation → Shape Variation Probability |
| `F2` | Minimum |
| `F3` | Maximum |
| `F4` | Gate → Gate Probability |
| `F5` | unused in the Curve step-layer map |

Reference pages:

- [Shape](curve-steps.md), [Shape Variation](curve-shape-variation.md), [Shape Variation Probability](curve-shape-variation-probability.md)
- [Minimum](curve-min.md)
- [Maximum](curve-max.md)
- [Gate](curve-gate.md), [Gate Probability](curve-gate-probability.md)

## Shift layer shortcuts

- [`SHIFT` + `F1` → Shape](curve-shift-shape.md)
- [`SHIFT` + `F2` → Minimum](curve-shift-min.md)
- [`SHIFT` + `F3` → Maximum](curve-shift-max.md)
- [`SHIFT` + `F4` → Gate](curve-shift-gate.md)

## Quick Edit

Curve Quick Edit keeps the Steps page visible while exposing sequence-level playback parameters.

| Step key | Parameter |
| --- | --- |
| `S9` | [First Step](curve-quick-first-step.md) |
| `S10` | [Last Step](curve-quick-last-step.md) |
| `S11` | [Run Mode](curve-quick-run-mode.md) |
| `S12` | [Divisor](curve-quick-divisor.md) |
| `S13` | [Reset Measure](curve-quick-reset-measure.md) |
| `S14` | [Voltage Range](curve-quick-range.md) |

## Context menu

[The Curve Steps context menu](curve-context-menu.md) operates on the current sequence/step context and exposes the actions available for editing, clipboard use and generation.

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
