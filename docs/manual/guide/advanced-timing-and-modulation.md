<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Advanced Timing and Modulation

## Clock and sync boundaries

Clock setup and project **Sync Measure** affect when synced Pattern, Song, and Performance requests execute.
Use [Clock Setup](../screens/global/clock.md) plus Project timing settings to align transitions to musical bars.

## Divisor, run mode, and reset measure

Sequence-level timing behavior comes from:

- **Divisor** (step-rate relative to clock)
- **Run Mode** (direction/ordering behavior)
- **Reset Measure** (periodic restart behavior)

Use Quick Edit on step pages for fast live adjustments.

## Microtiming and retrigger

- **Gate Offset** moves note onset relative to the grid
- negative offsets can pre-trigger when timing context allows it
- at hard boundaries, Styr triggers on the boundary instead
- **Retrigger** adds repeated pulses inside a step

Use these after basic note placement to avoid fighting timing while composing.

## Probability, variation, and condition layers

Note and Curve layers support controlled variation without writing multiple patterns:

- gate/shape probability
- note/length/shape variation ranges
- condition-based step execution

Build a stable core loop first, then add controlled variability.

## LFO Track as modulation source

Use [LFO Track](../features/lfo/README.md) for continuous modulation:

- Sync mode for tempo-relative cycles
- Free mode for Hz-based motion
- Min/Max range shaping for destination-safe voltages
- optional clip and waveform-specific controls
