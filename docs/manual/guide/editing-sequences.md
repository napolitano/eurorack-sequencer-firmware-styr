<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Editing Sequences

## Note Track editing workflow

Use [Note Steps](../screens/note/note-steps.md) as the main editor, then move through layer groups with `F1`…`F5`.

- `F1`: Gate family (Gate, Gate Probability, Gate Offset, Slide)
- `F2`: Retrigger family
- `F3`: Length family
- `F4`: Note family
- `F5`: Condition

Tips:

- Repeated presses cycle within a group.
- `SHIFT` + function key jumps to the group's primary layer.
- `SHIFT` + `PAGE` opens the context menu for init/copy/paste/duplicate/generators.

## Curve Track editing workflow

On [Curve Steps](../screens/curve/curve-steps.md), edit per-step shape and voltage behavior:

- shape and shape variation
- minimum and maximum voltage
- optional gate and gate probability

Use Curve Quick Edit overlays for sequence settings without leaving the Steps page.

## Quick Edit

On Note and Curve step pages, hold `PAGE` plus step keys (`S9`…`S15`) to temporarily edit common sequence settings such as first/last step, run mode, divisor, reset measure, and scale/range.

## Generators

Generators are fast ways to create or transform content:

- **Init Layer** (immediate action)
- **Euclidean**
- **Random**
- **Acid Bassline** (Note Track)

Use the generator preview, then **Commit** to keep results or **Revert** to restore the previous state. See [Generator reference](../screens/generators/README.md) and [Acid Bassline guide](../features/acid-bassline/README.md).

## Microtiming boundary behavior

Negative Gate Offset can move notes earlier than their nominal boundary. At transport/reset boundaries where no earlier event can be scheduled, Styr plays at the boundary instead.
