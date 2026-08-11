<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Note Track Screen Reference

A Note track sequences gates and pitch-oriented note data. Each sequence contains up to 64 steps, shown 16 at a time on the Steps page. Sequence-level playback parameters live on the Sequence page and are also available through Quick Edit from the Steps workflow.

## Track and sequence pages

- [Note Track](note-track.md) — track-wide playback and performance parameters.
- [Note Sequence](note-sequence.md) — First Step, Last Step, Run Mode, Divisor, Reset Measure, Scale and Root Note.

## Step layers

The five function keys select layer groups. Repeated presses cycle within a group; `SHIFT` + the same function key jumps directly to the group’s primary layer.

| Function key | Layer group |
| --- | --- |
| `F1` | Gate → Gate Probability → Gate Offset → Slide |
| `F2` | Retrigger → Retrigger Probability |
| `F3` | Length → Length Variation Range → Length Variation Probability |
| `F4` | Note → Note Variation Range → Note Variation Probability |
| `F5` | Condition |

Reference pages:

- [Gate](note-steps.md), [Gate Probability](note-gate-probability.md), [Gate Offset](note-gate-offset.md), [Slide](note-slide.md)
- [Retrigger](note-retrigger.md), [Retrigger Probability](note-retrigger-probability.md)
- [Length](note-length.md), [Length Variation Range](note-length-variation-range.md), [Length Variation Probability](note-length-variation-probability.md)
- [Note](note-note.md), [Note Variation Range](note-note-variation-range.md), [Note Variation Probability](note-note-variation-probability.md)
- [Condition](note-condition.md)

## Shift layer shortcuts

- [`SHIFT` + `F1` → Gate](note-shift-gate.md)
- [`SHIFT` + `F2` → Retrigger](note-shift-retrigger.md)
- [`SHIFT` + `F3` → Length](note-shift-length.md)
- [`SHIFT` + `F4` → Note](note-shift-note.md)
- [`SHIFT` + `F5` → Condition](note-shift-condition.md)

## Quick Edit

Hold `PAGE` and one of `S9`…`S15` to overlay a common sequence parameter without leaving the Steps page. Keep `PAGE` held while adjusting the value; releasing the chord returns to normal step editing.

| Step key | Parameter |
| --- | --- |
| `S9` | [First Step](note-quick-first-step.md) |
| `S10` | [Last Step](note-quick-last-step.md) |
| `S11` | [Run Mode](note-quick-run-mode.md) |
| `S12` | [Divisor](note-quick-divisor.md) |
| `S13` | [Reset Measure](note-quick-reset-measure.md) |
| `S14` | [Scale](note-quick-scale.md) |
| `S15` | [Root Note](note-quick-root-note.md) |

## Context menu

[The Steps context menu](note-context-menu.md) is opened with `SHIFT` + `PAGE` and provides operations on the current step selection, including initialization, clipboard actions, duplication and generator access where available.

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
